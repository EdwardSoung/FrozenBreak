// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/InventoryItemSlot.h"
#include "Objects/InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Selected->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryItemSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UInventoryItem* ItemData = Cast<UInventoryItem>(ListItemObject))
	{
		if (ItemData->GetData()->ItemIcon)
		{
			Icon->SetBrushFromTexture(ItemData->GetData()->ItemIcon.Get());
		}
		
		AmountText->SetText(FText::AsNumber(ItemData->GetAmount()));
	}	

	if (Selected)
	{
		Selected->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UInventoryItemSlot::NativeOnItemSelectionChanged(bool bIsSelected)
{
	if (Selected)
	{
		Selected->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
