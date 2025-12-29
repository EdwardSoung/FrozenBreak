// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/PlayResultWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "GameSystem/FrozenForestGameState.h"

#include "Data/Enums.h"

#include <Kismet/KismetSystemLibrary.h>

void UPlayResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LobbyButton)
	{
		LobbyButton->OnClicked.AddDynamic(this, &UPlayResultWidget::OnClickLobby);		
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UPlayResultWidget::OnClickExit);
	}
}


void UPlayResultWidget::UpdateRecord(int32 InDay, int32 InHour, int32 InMin)
{
	FString TimeText = TEXT("");
	if (InDay > 0)
	{
		TimeText = FString::Printf(TEXT("%d일 "), InDay);
	}
	if (InHour > 0)
	{
		TimeText = FString::Printf(TEXT("%02d시간 "), InHour);
	}
	RecordText->SetText(FText::FromString(TimeText + FString::Printf(TEXT("%02d분"), InMin)));
}

UPlayResultWidget* UPlayResultWidget::SetResult(bool IsClear)
{
	FString Result = IsClear ? TEXT("탈출 성공") : TEXT("탈출 실패");
	ResultText->SetText(FText::FromString(Result));
	return this;
}

void UPlayResultWidget::OnClickLobby()
{
	auto State = GetWorld()->GetGameState<AFrozenForestGameState>();
	if (State)
	{
		State->SetGameState(EGameState::Lobby);
	}
}

void UPlayResultWidget::OnClickExit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}
