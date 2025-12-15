// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ThirdPersonPlayerController.h"
#include "GameSystem/UISubSystem.h"

void AThirdPersonPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->ShowWidget(EWidgetType::HUD);
	}
}
