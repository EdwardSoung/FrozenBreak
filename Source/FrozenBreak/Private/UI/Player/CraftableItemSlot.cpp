// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/CraftableItemSlot.h"
#include "Objects/InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include <GameSystem/EventSubSystem.h>

void UCraftableItemSlot::NativeOnInitialized()
{
	CraftSelected->SetVisibility(ESlateVisibility::Hidden);
}

void UCraftableItemSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UInventoryItem* ItemData = Cast<UInventoryItem>(ListItemObject))
	{
		if (ItemData->GetData()->ItemIcon)
		{
			CraftItemIcon->SetBrushFromTexture(ItemData->GetData()->ItemIcon.Get());
		}

		CraftableAmount->SetText(FText::AsNumber(ItemData->GetAmount()));
	}

	if (CraftSelected)
	{
		CraftSelected->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCraftableItemSlot::NativeOnItemSelectionChanged(bool bIsSelected)
{
	UE_LOG(LogTemp, Log, TEXT("bIsSelected : %d"), bIsSelected);
	if (CraftSelected)
	{
		CraftSelected->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
