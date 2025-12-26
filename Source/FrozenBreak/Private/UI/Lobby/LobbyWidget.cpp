// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LobbyWidget.h"
#include "Components/Button.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Kismet/GameplayStatics.h"
#include "GameSystem/GameManager.h"
#include "GameSystem/UISubSystem.h"
#include "GameFramework/GameModeBase.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickStart);	
	ExitButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickExit);
}

void ULobbyWidget::OnClickStart()
{
	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->HideWidget(EWidgetType::Lobby);
	}

	UGameplayStatics::OpenLevel(this, "FrozenForest", true);
}

void ULobbyWidget::OnClickExit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}
