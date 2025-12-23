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

	// ...
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Character.OnGetPickupItem.AddDynamic(this, &UInventoryComponent::AddItem);
		EventSystem->Character.OnRequestInventoryInit.AddDynamic(this, &UInventoryComponent::InitInventoryUI);
		EventSystem->Character.OnTrashItem.AddDynamic(this, &UInventoryComponent::TrashItem);
		EventSystem->Character.OnRemoveItem.AddDynamic(this, &UInventoryComponent::TrashItem);

		EventSystem->Character.OnRequestIventoryItems.AddDynamic(this, &UInventoryComponent::SendInventoryItems);
		EventSystem->Character.OnUsableItemUsed.AddDynamic(this, &UInventoryComponent::UseUsableItem);

		EventSystem->Character.OnRequestIventoryRawMeet.AddDynamic(this, &UInventoryComponent::SendRawMeetData);

		EventSystem->Character.OnUseInventoryItem.AddDynamic(this, &UInventoryComponent::UseInventoryItem);
	}

	//임시로 가방 강제 장착. 나중에 캐릭터 장착 쪽에 가방도 두면 좋을 것 같음..
	if (BagData && BagData->ItemType == EItemType::Bag)
	{
		if (BagData->Stats.Contains(EItemStatType::Weight))
		{
			InventoryMaxWeight = BagData->Stats[EItemStatType::Weight];
		}
	}
}

void UInventoryComponent::SendInventoryItems()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Character.OnSendInventoryData.Broadcast(Items);
	}
}

void UInventoryComponent::UseUsableItem(UItemData* InData)
{
	for (auto Item : Items)
	{
		if (Item->GetType() == InData->ItemType)
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
			if (SingleItem->GetData()->ItemType == EItemType::RawMeat)
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
	EItemType InType = InItem->GetData()->ItemType;
	switch (InType)
	{
	case EItemType::None:
		break;
	case EItemType::RawMeat:
		break;
	case EItemType::CookedMeat:
		break;
	case EItemType::Fruit:
		break;
	case EItemType::Campfire:
		SpawnCampfire();
		break;
	default:
		break;
	}
}

void UInventoryComponent::SpawnCampfire()
{
	if (CampfireClass)
	{
		AActor* OwnerActor = GetOwner();
		if (!OwnerActor || !CampfireClass) return;

		const float SpawnDistance = 150.f;

		FVector Start =
			OwnerActor->GetActorLocation() +
			OwnerActor->GetActorForwardVector() * SpawnDistance;

		FVector End = Start - FVector(0, 0, 500.f);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerActor);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			Hit, Start, End, ECC_Visibility, Params
		);

		FVector SpawnLocation = bHit ? Hit.Location : Start;
		FRotator SpawnRotation = FRotator(0.f, OwnerActor->GetActorRotation().Yaw, 0.f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerActor;
		SpawnParams.Instigator = OwnerActor->GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		GetWorld()->SpawnActor<ABuffableWorldProp>(
			CampfireClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);
	}
}

//우선은...중복 허용하지 않음
void UInventoryComponent::AddItem(EItemType Type, int32 Amount)
{	
	if (CurrentWeight >= InventoryMaxWeight)
	{
		//무게 초과라 안먹어져야 함
		return;
	}

	if (UInventoryItem* TargetItem = GetItem(Type))
	{
		//장비류면 그냥 새로 추가해야.. uid 부여해서 장착 시 처리
		TargetItem->AddAmount(Amount);
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnUpdateInventoryItem.Broadcast(Type);
		}
	}
	else
	{
		if (UItemFactorySubSystem* ItemFactory = UItemFactorySubSystem::Get(this))
		{
			UInventoryItem* NewItem = ItemFactory->Spawn(Type, Amount);
			Items.Add(NewItem);

			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnAddItemToInventoryUI.Broadcast(NewItem);
			}
		}
	}
	RefreshWeight();
}

void UInventoryComponent::TrashItem(UInventoryItem* InItem)
{
	Items.Remove(InItem);
	RefreshWeight();
}
//동일 타입의 아이템이 존재하는지...
UInventoryItem* UInventoryComponent::GetItem(EItemType Type)
{
	if (Type == EItemType::Axe || Type == EItemType::Pickaxe || Type == EItemType::Knife)
	{
		//장비류는 동일아이템이 없다.
		//TODO : 의복도 추가..이거 그냥 아이템 그룹을 나누는게 나을지
		return nullptr;
	}
	for (auto Item : Items)
	{
		if (Item->GetData()->ItemType == Type)
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
