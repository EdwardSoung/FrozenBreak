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
	}

	if (HandEquipment)
	{
		HandEquipment->RefreshSlot(DefaultItemIcon, FText::FromString(TEXT("장비 1")));

	}
	if (BodyEquipment)
	{
		BodyEquipment->RefreshSlot(DefaultItemIcon, FText::FromString(TEXT("장비 2")));
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

