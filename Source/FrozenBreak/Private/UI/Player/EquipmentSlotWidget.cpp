// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/EquipmentSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Objects/InventoryItem.h"
#include "GameSystem/EventSubSystem.h"

void UEquipmentSlotWidget::RefreshSlot(UInventoryItem* InItem)
{
	if (InItem)
	{
		ItemIcon->SetVisibility(ESlateVisibility::Visible);
		auto ItemData = InItem->GetData();
		if (ItemData->ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(ItemData->ItemIcon.Get());
		}
		if (!ItemData->DisplayName.IsEmpty())
		{
			ItemName->SetText(ItemData->DisplayName);
		}
	}
	else
	{
		//빈칸으로 세팅해줘야함
		ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		ItemName->SetText(FText::FromString(TEXT("없음")));
	}
}

void UEquipmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
