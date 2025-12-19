// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/InventoryComponent.h"
#include "GameSystem/EventSubSystem.h"
#include "Objects/InventoryItem.h"

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
		EventSystem->Chraracter.OnGetPickupItem.AddDynamic(this, &UInventoryComponent::AddItem);
		EventSystem->Chraracter.OnRequestInventoryInit.AddDynamic(this, &UInventoryComponent::InitInventoryUI);
		EventSystem->Chraracter.OnTrashItem.AddDynamic(this, &UInventoryComponent::TrashItem);

		EventSystem->Chraracter.OnRequestIventoryItems.AddDynamic(this, &UInventoryComponent::SendInventoryItems);

		EventSystem->Chraracter.OnRequestIventoryRawMeet.AddDynamic(this, &UInventoryComponent::SendRawMeetData);
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
		EventSystem->Chraracter.OnSendInventoryData.Broadcast(Items);
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
		EventSystem->Chraracter.OnUpdateInventoryWeight.Broadcast(CurrentWeight, InventoryMaxWeight);
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
			EventSystem->Chraracter.OnSendRawMeet.Broadcast(Meets);
		}
	}
}

//우선은...중복 허용하지 않음
void UInventoryComponent::AddItem(EItemType Type, int32 Amount)
{	
	if (CurrentWeight >= InventoryMaxWeight)
	{
		return;
	}
	UInventoryItem* AddedItem = nullptr;
	if (UInventoryItem* TargetItem = GetItem(Type))
	{
		TargetItem->AddAmount(Amount);
		AddedItem = TargetItem;
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Chraracter.OnUpdateInventoryItem.Broadcast(Type);
		}
	}
	else
	{
		auto NewItem = NewObject<UInventoryItem>(this);
		auto NewData = DataList->ItemAssetData.Find(Type)->Get();
		NewItem->Initialize(NewData);
		NewItem->AddAmount(Amount);
		Items.Add(NewItem);
		AddedItem = NewItem;

		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Chraracter.OnAddItemToInventoryUI.Broadcast(AddedItem);
		}
	}
	RefreshWeight();
}

void UInventoryComponent::TrashItem(UInventoryItem* InItem)
{
	Items.Remove(InItem);
	RefreshWeight();
}

UInventoryItem* UInventoryComponent::GetItem(EItemType Type)
{
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
			EventSystem->Chraracter.OnInitInventoryUI.Broadcast(Items);
		}
	}
	RefreshWeight();
}
