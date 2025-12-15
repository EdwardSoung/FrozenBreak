// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/PlayerStatusWidget.h"
#include "Components/TextBlock.h"
#include "GameSystem/EventSubSystem.h"
#include "UI/Player/EquipmentSlotWidget.h"

void UPlayerStatusWidget::NativeConstruct()
{
	SetInGameTime(DefaultGameTime);
	SetTemperatureDefence(DefaultTemperatureDefence);

	if (UEventSubSystem* statusEvent = UEventSubSystem::GetEventSystem(this))
	{
		statusEvent->Status.OnInGameTimeChanged.AddDynamic(this, &UPlayerStatusWidget::SetInGameTime);
		statusEvent->Status.OnTemperatureDefenceChanged.AddDynamic(this, &UPlayerStatusWidget::SetTemperatureDefence);
		statusEvent->Status.OnHandEquipmentChanged.AddDynamic(this, &UPlayerStatusWidget::SetHandEquipment);
		statusEvent->Status.OnBodyEquipmentChanged.AddDynamic(this, &UPlayerStatusWidget::SetBodyEquipment);
	}
}

void UPlayerStatusWidget::SetInGameTime(FText InTimeValue)
{
	if (InGameTime)
	{
		InGameTime->SetText(InTimeValue);
	}
}

void UPlayerStatusWidget::SetTemperatureDefence(FText InDefenceValue)
{
	if (TemperatureDefence)
	{
		TemperatureDefence->SetText(InDefenceValue);
	}
}

void UPlayerStatusWidget::SetHandEquipment(FText InHandItem)
{
	if (HandEquipment)
	{
		HandEquipment->RefreshSlot(DefaultItemIcon, InHandItem);
	}
}

void UPlayerStatusWidget::SetBodyEquipment(FText InBodyItem)
{
	if (BodyEquipment)
	{
		BodyEquipment->RefreshSlot(DefaultItemIcon, InBodyItem);
	}
}

