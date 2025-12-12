// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/EquipmentSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameSystem/EventSubSystem.h"

void UEquipmentSlotWidget::RefreshSlot(UTexture2D* InIcon, FText InName)
{
	ItemIcon->SetBrushFromTexture(InIcon);
	ItemName->SetText(InName);
}

void UEquipmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
