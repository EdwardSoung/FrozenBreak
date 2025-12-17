// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/StatusDataAsset.h"
#include "StatusCalculationSubSystem.generated.h"

/**
 * 스테이터스의 증감 값을 계산하여 최종 값을 컴포넌트로 보내는 서브시스템.
 * 이곳에서는 지속적으로 들어가는 값(자연적인 굶주림, 모닥불, 눈보라 등 환경에 의한 증감)만 계산하여 보낸다.
 * 특정 활동으로 인한 1회성 값(음식 섭취 등)은 OnSet 델리게이트를 즉시 Broadcast하여 반영.
 * 
 * Increase를 사용하면 현재 증감 값에 +
 * Decrease를 사용하면 현재 증감 값에 -
 * 
 * 예)  1. 온도 Default 값을 -0.5로 설정하면 지속적으로 0.5씩 깎임
 *		2. 모닥불과 Begin Overlap이 일어날 시 모닥불에서 본 서브시스템의 IncreaseTemperature(2.0) 함수를 호출하면 최종값이 1.5가 되어 지속적으로 1.5씩 회복
 *		3. 모닥불과 End Overlap이 일어날 시 모닥불에서 DecreaseTemperature(2.0) 함수를 호출하면 다시 최종값이 -0.5가 되어 지속적으로 감소
 *
 * 방한값 또한 옷 착용 / 해제 시 IncreaseTemperature / DecreaseTemperature 함수를 통해 지속 증감 값에 영향을 행사하도록 한다. 
 */
UCLASS()
class FROZENBREAK_API UStatusCalculationSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// 현재는 해당 데이터 애셋 셋팅 위치는 레벨 블루프린트(테스트용)
	// TODO : 적합한 위치에서 Set 필요
	UFUNCTION(BlueprintCallable)
	inline void SetStatusDataAsset(UStatusDataAsset* InDataAsset) { StatusDataAsset = InDataAsset; }

	// 체온 값 +,-
	UFUNCTION(BlueprintCallable)
	inline void IncreaseTemperature(float InValue) { if (StatusDataAsset) StatusDataAsset->Temperature += InValue; }
	UFUNCTION(BlueprintCallable)
	inline void DecreaseTemperature(float InValue) { if (StatusDataAsset) StatusDataAsset->Temperature -= InValue; }

	// 활동력 값 +,-
	UFUNCTION(BlueprintCallable)
	inline void IncreaseFatigue(float InValue) { if (StatusDataAsset) StatusDataAsset->Fatigue += InValue; }
	UFUNCTION(BlueprintCallable)
	inline void DecreaseFatigue(float InValue) { if (StatusDataAsset) StatusDataAsset->Fatigue -= InValue; }

	// 포만감 값 +,-
	UFUNCTION(BlueprintCallable)
	inline void IncreaseHunger(float InValue) { if (StatusDataAsset) StatusDataAsset->Hunger += InValue; }
	UFUNCTION(BlueprintCallable)
	inline void DecreaseHunger(float InValue) { if (StatusDataAsset) StatusDataAsset->Hunger -= InValue; }

	UFUNCTION(BlueprintCallable)
	void StartStatLoop();

private:
	// 체온 지속 셋팅 타이머 시작
	void StartTemperatureLoop();

	// 활동력 지속 셋팅 타이머 시작
	void StartFatigueLoop();

	// 포만감 지속 셋팅 타이머 시작
	void StartHungerLoop();

	void SetTemperature();
	void SetFatigue();
	void SetHunger();

protected:
	UPROPERTY()
	class UEventSubSystem* EventSystem = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStatusDataAsset> StatusDataAsset = nullptr;
private:
	FTimerHandle TemperatureTimer;
	FTimerHandle FatigueTimer;
	FTimerHandle HungerTimer;
};
