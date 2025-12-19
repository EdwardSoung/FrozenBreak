// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/QuitMenuWidget.h"
#include "Components/Button.h"
#include <GameSystem/EventSubSystem.h>

void UQuitMenuWidget::NativeConstruct()
{
	ResumeButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnResumeClicked);
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
	//UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit);
	//씬 전환..메뉴로
}
