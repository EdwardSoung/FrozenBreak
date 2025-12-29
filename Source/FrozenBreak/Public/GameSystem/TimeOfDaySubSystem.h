// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TimeOfDaySubSystem.generated.h"


/**
 * 시간 흐름 컨트롤 서브시스템
 * 시간에 따라 directional light 회전과 시간 값 델리게이트를 전달
 * 현재 시간 비율은 720초(12분)에 24시간으로 설정
 */
UCLASS()
class FROZENBREAK_API UTimeOfDaySubSystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	// Subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Tick
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bEnableTimeFlow && IsGameWorldSafe(); }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UInGameTimeSubsystem, STATGROUP_Tickables); }

	int32 GetDay() { return Day; }

	UFUNCTION(BlueprintCallable)
	void SetTimeNormalized(float InNormalized);

	UFUNCTION(BlueprintCallable)
	float GetTimeNormalized() const { return TimeNormalized; }

	// 인게임 시간 N시간 즉시 진행 (예: Sleep 4시간)
	UFUNCTION(BlueprintCallable, Category = "Time")
	void SkipTimeByHours(float Hours);

private:
	// === Internal ===
	void UpdateDirectionalLight();
	void BroadcastTimeIfMinuteChanged();
	FText MakeTimeText24H() const;
	bool IsGameWorldSafe() const;
	void FindDirectionalLight();

protected:
	UPROPERTY()
	class UEventSubSystem* EventSystem = nullptr;

	class AFrozenForestGameState* GameState = nullptr;
private:
	// 시간 (0~1)
	float TimeNormalized = 0.25f; // 06:00 시작

	// 하루 길이 (현실 초)
	float DayLengthSeconds = 720.0f;

	bool bEnableTimeFlow = true;

	// 캐시
	int32 CachedMinute = -1;

	TWeakObjectPtr<class ADirectionalLight> CachedDirectionalLight;

	float SunPitchOffsetDeg = 15.0f; // 6시에 태양을 지평선보다 15도 위로

	int32 Day = 1;
};
