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
		//음..인벤토리 저장되면?
		if (Items.Num() > 0)
		{
			EventSystem->Chraracter.OnInitInventoryUI.Broadcast(Items);
		}
		EventSystem->Chraracter.OnGetPickupItem.AddDynamic(this, &UInventoryComponent::AddItem);
	}

}

//우선은...중복 허용하지 않음
void UInventoryComponent::AddItem(EItemType Type, int32 Amount)
{
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
