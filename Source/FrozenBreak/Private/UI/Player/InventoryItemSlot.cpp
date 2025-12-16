// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/InventoryItemSlot.h"
#include "Components/Image.h"

void UInventoryItemSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
}

void UInventoryItemSlot::NativeOnItemSelectionChanged(bool bIsSelected)
{
	Selected->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
