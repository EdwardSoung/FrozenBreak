// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/FrozenForestGameState.h"
#include "GameSystem/UISubSystem.h"
#include <Kismet/GameplayStatics.h>

AFrozenForestGameState::AFrozenForestGameState()
{
	CurrentState = EGameState::Lobby;
}

void AFrozenForestGameState::SetGameState(EGameState InState)
{
	CurrentState = InState;

	OnGameStateChanged();
}

void AFrozenForestGameState::OnGameStateChanged()
{
	switch (CurrentState)
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
		//탈출 성공 시
		//플레이어 조작 막기
		//성공 애니메이션
		//기록 표기? x일 x시간x분 만에 탈출
		break;
	case EGameState::Fail:
		//사망 시
		//플레이어 조작 막기
		//재시작 UI
		break;
	}
}
