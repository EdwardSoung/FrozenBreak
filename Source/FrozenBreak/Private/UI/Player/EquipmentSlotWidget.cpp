// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/EquipmentSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameSystem/EventSubSystem.h"

void UEquipmentSlotWidget::RefreshSlot(UTexture2D* InIcon, FText InName)
{
	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(InIcon);
	}
	if (!InName.IsEmpty())
	{
		ItemName->SetText(InName);
	}
}

void UEquipmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
