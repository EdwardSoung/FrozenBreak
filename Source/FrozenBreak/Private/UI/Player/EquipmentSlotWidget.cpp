// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/EquipmentSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameSystem/EventSubSystem.h"

void UEquipmentSlotWidget::RefreshSlot(UItemData* InItemData)
{
	if (InItemData)
	{
		if (InItemData->ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(InItemData->ItemIcon.Get());
		}
		if (!InItemData->DisplayName.IsEmpty())
		{
			ItemName->SetText(InItemData->DisplayName);
		}
	}
}

void UEquipmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
