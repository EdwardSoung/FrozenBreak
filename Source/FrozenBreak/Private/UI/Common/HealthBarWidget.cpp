// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/HealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "GameSystem/EventSubSystem.h"

void UHealthBarWidget::SetHealthProgress(float InValue)
{
	if (HealthProgress)
	{
		HealthProgress->SetPercent(InValue);
	}
}

void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetHealthProgress(InitValue);

	if (UEventSubSystem* statusEvent = UEventSubSystem::Get(this))
	{
		statusEvent->Status.OnHealthPointChanged.AddDynamic(this, &UHealthBarWidget::SetHealthProgress);
	}
}