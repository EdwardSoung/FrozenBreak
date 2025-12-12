// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/TemperatureBarWidget.h"
#include "Components/ProgressBar.h"
#include "GameSystem/EventSubSystem.h"

void UTemperatureBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetTemperatureProgress(InitValue);

	if (UEventSubSystem* statusEvent = UEventSubSystem::GetEventSystem(this))
	{
		statusEvent->Status.OnTemperaturePointChanged.AddDynamic(this, &UTemperatureBarWidget::SetTemperatureProgress);
	}
}

void UTemperatureBarWidget::SetTemperatureProgress(float InValue)
{
	if (TemperatureProgress)
	{
		TemperatureProgress->SetPercent(InValue);
	}
}
