// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/QuitMenuWidget.h"
#include "Components/Button.h"
#include "GameSystem/FrozenForestGameState.h"
#include <GameSystem/EventSubSystem.h>

void UQuitMenuWidget::NativeConstruct()
{
	ResumeButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnResumeClicked);
	QuitButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnQuitClicked);
}

void UQuitMenuWidget::OnResumeClicked()
{
	//음..이벤트로?
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->UI.OnToggleQuitMenu.Broadcast();
	}
}

void UQuitMenuWidget::OnQuitClicked()
{
	auto State = GetWorld()->GetGameState<AFrozenForestGameState>();
	if (State)
	{
		State->SetGameState(EGameState::Lobby);
	}
}
