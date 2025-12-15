// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/HungerBarWidget.h"
#include "Components/ProgressBar.h"
#include "GameSystem/EventSubSystem.h"

void UHungerBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetHungerProgress(InitValue);

	if (UEventSubSystem* statusEvent = UEventSubSystem::Get(this))
	{
		statusEvent->Status.OnHungerPointChanged.AddDynamic(this, &UHungerBarWidget::SetHungerProgress);
	}
}

void UHungerBarWidget::SetHungerProgress(float InValue)
{
	if (HungerProgress)
	{
		HungerProgress->SetPercent(InValue);
	}
}
