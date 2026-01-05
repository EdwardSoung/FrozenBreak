// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LobbyController.h"
#include "GameSystem/UISubSystem.h"

//마우스 위치의 오브젝트중에 침대/문에 따라 메뉴 표기
//해당 오브젝트 누르면 동작

void ALobbyController::BeginPlay()
{
	Super::BeginPlay();

	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->ClearAllWidgets();

		UISystem->ShowWidget(EWidgetType::Lobby);
	}
}
