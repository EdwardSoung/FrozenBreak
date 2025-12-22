// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Prop/PropDurabilityWidget.h"
#include "Components/ProgressBar.h"
#include "GameSystem/EventSubSystem.h"

void UPropDurabilityWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPropDurabilityWidget::SetDurabilityProgress(float InValue)
{
	if (DurabilityProgress)
	{
		DurabilityProgress->SetPercent(InValue);
	}
}
