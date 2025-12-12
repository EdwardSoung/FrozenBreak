// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/FatigueBarWidget.h"
#include "Components/ProgressBar.h"
#include "GameSystem/EventSubSystem.h"

void UFatigueBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetFatigueProgress(InitValue);

	if (UEventSubSystem* statusEvent = UEventSubSystem::GetEventSystem(this))
	{
		statusEvent->Status.OnFatiguePointChanged.AddDynamic(this, &UFatigueBarWidget::SetFatigueProgress);
	}
}

void UFatigueBarWidget::SetFatigueProgress(float InValue)
{
	if (FatigueProgress)
	{
		FatigueProgress->SetPercent(InValue);
	}
}
