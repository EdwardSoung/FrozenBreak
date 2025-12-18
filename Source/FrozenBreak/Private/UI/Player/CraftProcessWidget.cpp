// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/CraftProcessWidget.h"
#include "Components/ProgressBar.h"
#include <GameSystem/EventSubSystem.h>

void UCraftProcessWidget::NativeConstruct()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Status.OnCurrentCraftCostChanged.AddDynamic(this, &UCraftProcessWidget::SetCraftProcess);
	}
	SetCraftProcess(DefaultProgress);
}

void UCraftProcessWidget::SetCraftProcess(float InCurrentProgress)
{
	if (CraftProcess)
	{
		CraftProcess->SetPercent(InCurrentProgress);
;	}
}
