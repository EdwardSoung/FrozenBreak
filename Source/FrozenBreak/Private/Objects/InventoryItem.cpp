// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/InventoryItem.h"

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
}

void UInventoryItem::SetAmount(int32 InAmount)
{
	Amount = InAmount;
}