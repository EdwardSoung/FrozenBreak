// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LobbyWidget.h"

#include "Components/Button.h"

#include <Kismet/KismetSystemLibrary.h>

#include "GameSystem/FrozenForestGameState.h"

#include "GameFramework/GameModeBase.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickStart);	
	ExitButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickExit);
}

void ULobbyWidget::OnClickStart()
{
	auto State = GetWorld()->GetGameState<AFrozenForestGameState>();
	if (State)
	{
		State->SetGameState(EGameState::Playing);
	}
}

void ULobbyWidget::OnClickExit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}
