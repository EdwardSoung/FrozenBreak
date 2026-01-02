// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/TimeOfDaySubSystem.h"
#include "GameSystem/EventSubSystem.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/DirectionalLight.h"
#include "Components/LightComponent.h"
#include "GameSystem/FrozenForestGameState.h"
#include "GameSystem/GameManager.h"

void UTimeOfDaySubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FindDirectionalLight();
	EventSystem = UEventSubSystem::Get(this);

	if (UGameManager* GameMgr = UGameManager::Get(this))
	{
		TimeNormalized = GameMgr->GetInGameStartTime();
		EventSystem->Status.OnDayChanged.Broadcast(Day);
	}
}

void UTimeOfDaySubSystem::Deinitialize()
{
	CachedDirectionalLight.Reset();
	Super::Deinitialize();
}

void UTimeOfDaySubSystem::Tick(float DeltaTime)
{
	if (auto Manager = UGameManager::Get(this))
	{
		if (Manager->GetGameState() != EGameState::Playing)
			return;
	}
	else
		return;

	if (DayLengthSeconds <= KINDA_SMALL_NUMBER) return;

	TimeNormalized += DeltaTime / DayLengthSeconds;
	if (TimeNormalized >= 1.0f)
	{
		Day++;
		EventSystem->Status.OnDayChanged.Broadcast(Day);
		TimeNormalized = FMath::Fmod(TimeNormalized, 1.0f);
	}

	UpdateDirectionalLight();
	BroadcastTimeIfMinuteChanged();
}

void UTimeOfDaySubSystem::FindDirectionalLight()
{
	if (!GetWorld()) return;

	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		// 첫 번째 DirectionalLight 사용
		CachedDirectionalLight = *It;
		break;
	}
}

void UTimeOfDaySubSystem::SkipTimeByHours(float Hours)
{
	// Hours가 음수면 되감기 느낌이 되지만, 보통은 0 이상만 허용
	if (FMath::IsNearlyZero(Hours)) return;

	const float DeltaNormalized = Hours / 24.0f;

	// 즉시 점프
	TimeNormalized = TimeNormalized + DeltaNormalized;

	if (TimeNormalized > 1.0f)
		Day++;

	TimeNormalized = FMath::Fmod(TimeNormalized, 1.0f);
	if (TimeNormalized < 0.0f) TimeNormalized += 1.0f;

	// 라이트/시간 UI 갱신
	UpdateDirectionalLight();
	BroadcastTimeIfMinuteChanged();
}

void UTimeOfDaySubSystem::UpdateDirectionalLight()
{
	if (!CachedDirectionalLight.IsValid())
	{
		FindDirectionalLight();
		return;
	}

	const float Pitch = 90.0f - (TimeNormalized * 360.0f);
	CachedDirectionalLight->SetActorRotation(FRotator(Pitch, 0.0f, 0.0f));
}

void UTimeOfDaySubSystem::BroadcastTimeIfMinuteChanged()
{
	const float TotalMinutesFloat = TimeNormalized * 24.0f * 60.0f;
	const int32 TotalMinutes = FMath::FloorToInt(TotalMinutesFloat);
	const int32 Minute = TotalMinutes % 60;

	if (Minute != CachedMinute)
	{
		CachedMinute = Minute;
		EventSystem->Status.OnInGameTimeChanged.Broadcast(MakeTimeText24H());
	}
}

FText UTimeOfDaySubSystem::MakeTimeText24H() const
{
	const int32 TotalMinutes = FMath::FloorToInt(TimeNormalized * 24.0f * 60.0f);
	const int32 Hour = (TotalMinutes / 60) % 24;
	const int32 Minute = TotalMinutes % 60;

	return FText::FromString(
		FString::Printf(TEXT("%02d:%02d"), Hour, Minute)
	);
}