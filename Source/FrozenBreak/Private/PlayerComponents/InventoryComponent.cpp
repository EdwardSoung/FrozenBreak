// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/InventoryComponent.h"
#include "GameSystem/EventSubSystem.h"
#include "GameSystem/ItemFactorySubSystem.h"
#include "Objects/InventoryItem.h"
#include "Objects/BuffableWorldProp.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	// ...
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInventoryComponent::InitializeComponent()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Character.OnGetPickupItem.AddDynamic(this, &UInventoryComponent::AddItem);
		EventSystem->Character.OnMoveItemToInventory.AddDynamic(this, &UInventoryComponent::MoveItem);
		EventSystem->Character.OnRequestInventoryInit.AddDynamic(this, &UInventoryComponent::InitInventoryUI);
		EventSystem->Character.OnRequestIventoryItems.AddDynamic(this, &UInventoryComponent::SendInventoryItems);
		EventSystem->Character.OnUpdateItem.AddDynamic(this, &UInventoryComponent::UpdateItem);
		EventSystem->Character.OnUseItem.AddDynamic(this, &UInventoryComponent::UseInventoryItem);
		EventSystem->Character.OnDropItem.AddDynamic(this, &UInventoryComponent::DropItem);

		EventSystem->Character.OnRequestIventoryRawMeet.AddDynamic(this, &UInventoryComponent::SendRawMeetData);

		EventSystem->UI.OnQuickSlotExecute.AddDynamic(this, &UInventoryComponent::QuickSlotExecute);
		EventSystem->UI.OnResetQuickSlotItem.AddDynamic(this, &UInventoryComponent::ResetQuickSlot);
	}

	//임시로 가방 강제 장착. 나중에 캐릭터 장착 쪽에 가방도 두면 좋을 것 같음..
	if (BagData && BagData->ItemType == EItemType::Bag)
	{
		if (BagData->Stats.Contains(EItemStatType::Weight))
		{
			InventoryMaxWeight = BagData->Stats[EItemStatType::Weight];
		}
	}

	Items.Empty();
	//최초 아이템 제공
	AddItem(EItemType::Axe, 1, 0);
	AddItem(EItemType::Axe, 1, 0);
	AddItem(EItemType::Pickaxe, 1, 0);
	AddItem(EItemType::Knife, 1, 0);
	AddItem(EItemType::RawMeat, 10, 0);
}

void UInventoryComponent::SendInventoryItems()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Character.OnSendInventoryData.Broadcast(Items);
	}
}


void UInventoryComponent::UseItem(int32 UID)
{
	for (auto Item : Items)
	{
		if (Item->GetUID() == UID)
		{
			Item->AddAmount(-1);
			break;
		}
	}

}

void UInventoryComponent::RefreshWeight()
{
	CurrentWeight = 0;
	for (auto Item : Items)
	{
		CurrentWeight += Item->GetWeight();
	}
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Character.OnUpdateInventoryWeight.Broadcast(CurrentWeight, InventoryMaxWeight);
	}
}

void UInventoryComponent::SendRawMeetData()
{
	if (!Items.IsEmpty())
	{
		TArray<UInventoryItem*> Meets;
		for (auto& SingleItem : Items)
		{
			if (SingleItem->GetData() && SingleItem->GetData()->ItemType == EItemType::RawMeat)
			{
				Meets.Add(SingleItem);
			}
		}
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnSendRawMeet.Broadcast(Meets);
		}
	}
}

void UInventoryComponent::UseInventoryItem(UInventoryItem* InItem)
{
	if (!InItem)
	{
		return;
	}

	EItemType InType = InItem->GetType();
	
	switch (InType)
	{
	case EItemType::Knife:
	case EItemType::Axe:
	case EItemType::Pickaxe:
		//우선은 장착은 따로 보내줌...캐릭터에서도 분기타기 좀
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnEquipHandItem.Broadcast(InItem);
		}
		break;
	case EItemType::Jaket:
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnEquipBodyItem.Broadcast(InItem);
		}
		break;
	case EItemType::Campfire:
		SpawnCampfire();
		break;
	default:
		break;
	}

	//공통적으로 해당 아이템 개수를 줄여줌
	//개수가 없으면 알아서 삭제 요청이 갈거임.
	UseItem(InItem->GetUID());

}

void UInventoryComponent::QuickSlotExecute(int32 InSlotNum)
{
	for (auto Item : Items)
	{
		if (Item->QuickSlotNum == InSlotNum)
		{
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnUseItem.Broadcast(Item);
				Item->QuickSlotNum = 0;
			}
			break;
		}
	}
}

void UInventoryComponent::ResetQuickSlot(int32 InSlotNum)
{
	for (auto Item : Items)
	{
		if (Item->QuickSlotNum == InSlotNum)
		{
			Item->QuickSlotNum = 0;
			break;
		}
	}
}

void UInventoryComponent::UpdateItem(UInventoryItem* InItem)
{
	for (auto Item : Items)
	{
		if (Item->GetUID() == InItem->GetUID())
		{
			if (InItem->GetAmount() == 0)
			{
				RemoveItem(Item);
			}
			else
			{
				//내구도, 개수 등 변경될게 많다...
				Item = InItem;
			}
			break;
		}
	}
	
}

void UInventoryComponent::SpawnCampfire()
{
	if (CampfireClass)
	{
		AActor* OwnerActor = GetOwner();
		if (!OwnerActor) return;

		const float SpawnDistance = 150.f;

		// 캐릭터 앞 위치
		FVector Start =
			OwnerActor->GetActorLocation() +
			OwnerActor->GetActorForwardVector() * SpawnDistance;

		FVector End = Start - FVector(0, 0, 500.f);

		// 바닥 라인트레이스
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerActor);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			Hit, Start, End, ECC_Visibility, Params
		);

		if (!bHit) return;

		// 위치
		FVector SpawnLocation = Hit.Location;

		// 경사면 기준 Forward 계산
		FVector OwnerForward = OwnerActor->GetActorForwardVector();

		// 바닥 평면에 투영
		FVector ProjectedForward = FVector::VectorPlaneProject(OwnerForward, Hit.Normal).GetSafeNormal();

		// 회전
		FRotator SpawnRotation =
			FRotationMatrix::MakeFromZX(Hit.Normal, ProjectedForward).Rotator();

		// Spawn
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerActor;
		SpawnParams.Instigator = OwnerActor->GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		GetWorld()->SpawnActor<ABuffableWorldProp>(
			CampfireClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);
	}
}

void UInventoryComponent::AddItem(EItemType InType, int32 InAmount, float InDurability)
{	
	if (CurrentWeight >= InventoryMaxWeight)
	{
		//무게 초과라 안먹어져야 함
		return;
	}
	UInventoryItem* TargetItem = GetItem(InType);
	if (TargetItem && TargetItem->GetData() && TargetItem->GetData()->IsStackable)
	{
		//누적가능한 아이템이면 개수 증가
		TargetItem->AddAmount(InAmount);
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnUpdateInventoryItem.Broadcast(InType);
		}
	}
	else
	{
		if (UItemFactorySubSystem* ItemFactory = UItemFactorySubSystem::Get(this))
		{
			UInventoryItem* NewItem = ItemFactory->Spawn(InType, InAmount, InDurability);
			Items.Add(NewItem);

			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnAddItemToInventoryUI.Broadcast(NewItem);
			}
		}
	}
	RefreshWeight();
}

void UInventoryComponent::MoveItem(UInventoryItem* InItem)
{
	Items.Add(InItem);

	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Character.OnAddItemToInventoryUI.Broadcast(InItem);
	}
	RefreshWeight();
}

void UInventoryComponent::RemoveItem(UInventoryItem* InItem)
{
	Items.Remove(InItem);
	RefreshWeight();
}
void UInventoryComponent::DropItem(UInventoryItem* InItem)
{
	if (UItemFactorySubSystem* ItemFactory = UItemFactorySubSystem::Get(this))
	{
		FVector GenLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.0f;
		ItemFactory->Spawn(InItem->GetType(), GenLocation, InItem->GetAmount(), InItem->GetDurability());
	}

	for (auto Item : Items)
	{
		if (Item->GetUID() == InItem->GetUID())
		{
			RemoveItem(Item);			
			break;
		}
	}
}
//동일 타입의 아이템이 존재하는지...
UInventoryItem* UInventoryComponent::GetItem(EItemType Type)
{
	for (auto Item : Items)
	{
		if (Item->GetData() && Item->GetData()->ItemType == Type)
		{
			return Item;
		}
	}

	return nullptr;
}

void UInventoryComponent::InitInventoryUI()
{
	if (Items.Num() > 0)
	{
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnInitInventoryUI.Broadcast(Items);
		}
	}
	RefreshWeight();
}
