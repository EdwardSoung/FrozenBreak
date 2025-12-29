// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/FrozenForestGameState.h"
#include "GameSystem/UISubSystem.h"
#include "GameSystem/GameManager.h"
#include "GameSystem/TimeOfDaySubSystem.h"

#include "UI/Common/PlayResultWidget.h"

#include <Kismet/GameplayStatics.h>

AFrozenForestGameState::AFrozenForestGameState()
{
	
}

void AFrozenForestGameState::SetGameState(EGameState InState)
{
	if (UGameManager* Manager = UGameManager::Get(this))
	{
		Manager->SetState(InState);

		OnGameStateChanged(InState);
	}

}

void AFrozenForestGameState::OnGameStateChanged(EGameState InState)
{
	switch (InState)
	{
	case EGameState::Lobby:
		if (UUISubSystem* UISystem = UUISubSystem::Get(this))
		{
			UISystem->HideAllWiget();
		}

		UGameplayStatics::OpenLevel(this, "Lobby", true);
		break;
	case EGameState::Playing:
		if (UUISubSystem* UISystem = UUISubSystem::Get(this))
		{
			UISystem->HideWidget(EWidgetType::Lobby);
		}

		UGameplayStatics::OpenLevel(this, "FrozenForest", true);
		//레벨 전환
		break;
	case EGameState::Success:
		if (UUISubSystem* UISystem = UUISubSystem::Get(this))
		{
			UISystem->HideAllWiget();

			auto Result = Cast<UPlayResultWidget>(UISystem->ShowWidget(EWidgetType::PlayResult));

			auto TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubSystem>();
			float TimeNormalized = TimeSystem->GetTimeNormalized();
			int32 RemainDay = TimeSystem->GetDay();
			const int32 TotalMinutes = FMath::FloorToInt(TimeNormalized * 24.0f * 60.0f);
			const int32 Hour = (TotalMinutes / 60) % 24;
			const int32 Minute = TotalMinutes % 60;

			Result->SetResult(true)->UpdateRecord(RemainDay - 1, Hour - 12, Minute);
		}
		break;
	case EGameState::Fail:
		//사망 시
		if (UUISubSystem* UISystem = UUISubSystem::Get(this))
		{
			UISystem->HideAllWiget();

			auto Result = Cast<UPlayResultWidget>(UISystem->ShowWidget(EWidgetType::PlayResult));

			auto TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubSystem>();
			float TimeNormalized = TimeSystem->GetTimeNormalized();
			int32 RemainDay = TimeSystem->GetDay();
			const int32 TotalMinutes = FMath::FloorToInt(TimeNormalized * 24.0f * 60.0f);
			const int32 Hour = (TotalMinutes / 60) % 24;
			const int32 Minute = TotalMinutes % 60;

			Result->SetResult(false)->UpdateRecord(RemainDay - 1, Hour - 12, Minute);
		}
		//재시작 UI
		break;
	}
}
