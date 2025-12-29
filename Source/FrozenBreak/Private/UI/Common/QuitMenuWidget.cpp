// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/QuitMenuWidget.h"
#include "Components/Button.h"
#include "GameSystem/FrozenForestGameState.h"
#include <GameSystem/EventSubSystem.h>
#include <Kismet/KismetSystemLibrary.h>

void UQuitMenuWidget::NativeConstruct()
{
	ResumeButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnResumeClicked);
	RecipeButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnRecipeClicked);
	QuitButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnQuitClicked);
	LobbyButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnLobbyClicked);
}

void UQuitMenuWidget::OnResumeClicked()
{
	//음..이벤트로?
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->UI.OnToggleQuitMenu.Broadcast();
	}
}

void UQuitMenuWidget::OnRecipeClicked()
{

}

void UQuitMenuWidget::OnLobbyClicked()
{
	auto State = GetWorld()->GetGameState<AFrozenForestGameState>();
	if (State)
	{
		State->SetGameState(EGameState::Lobby);
	}
}

void UQuitMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}
