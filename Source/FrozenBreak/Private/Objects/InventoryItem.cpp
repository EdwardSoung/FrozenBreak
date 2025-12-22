// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/InventoryItem.h"
#include "GameSystem/EventSubSystem.h"

void UInventoryItem::Initialize(uint32 InUID, UItemData* InData)
{
	UID = InUID;
	Data = InData;
	Amount = 0;
	MaxDurability = Data->Durability;
	Durability = MaxDurability;
}

void UInventoryItem::AddAmount(int32 InAmount)
{
	Amount += InAmount;
	if (Amount <= 0)
	{
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnTrashItem.Broadcast(this);
		}
	}
}

void UInventoryItem::SetAmount(int32 InAmount)
{
	Amount = InAmount;
}

void UInventoryItem::Use()
{
	if (Data->Stats.Contains(EItemStatType::UseDurability))
	{
		float useValue = *Data->Stats.Find(EItemStatType::UseDurability);
		Durability -= useValue;

		Durability = FMath::Clamp(Durability, 0, MaxDurability);

		if (Durability == 0)
		{
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnEquipInventoryItem.Broadcast(nullptr);
			}
		}
	}
}
