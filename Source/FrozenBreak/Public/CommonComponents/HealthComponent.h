// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonComponents/StatComponent.h"
#include "Components/AudioComponent.h"
#include "HealthComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROZENBREAK_API UHealthComponent : public UStatComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	// 체력 증감 함수
	UFUNCTION(BlueprintCallable, Category = "Stat|Health")
	void SetPlayerHealth(float InHealthValue);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 게임 시작 시 각 Current 값들을 Max 값으로 초기화
	void InitStatus();

	void BindStatusSettingEvents();

public:	

protected:
	// ------------------------------------------------------------------------------ Default 변수
	// 블루프린트에서 설정 가능하도록 만든 최대 HP MAX값
	// 부모 클래스인 StatComponent에 MaxHealth가 있긴 하지만 실제로 유효한 Health 값을 가지는 액터에게 아래 값 부여
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Health")
	float DefaultMaxHealth = 100.0f;

	// 각 수치의 최소값
	float DefaultMinimum = 0.0f;
	// ------------------------------------------------------------------------------|	

private:

protected:
	// 심장박동 사운드
	UPROPERTY(EditAnywhere, Category = "Stat|Health|Sound")
	USoundBase* LowHealthHeartbeatSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Stat|Health|Sound") // 체력이 20%에 가까우면 소리가 작아지고
	float HeartbeatVolumeMin = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Stat|Health|Sound") // 0%로 갈수록 소리가 커짐
	float HeartbeatVolumeMax = 0.9f;

	UPROPERTY(EditAnywhere, Category = "Stat|Health|Sound") // 피치로 긴박한 느낌조절
	float HeartbeatPitchMin = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Stat|Health|Sound")
	float HeartbeatPitchMax = 1.35f;

	// 실제 재생용 컴포넌트
	UPROPERTY()
	UAudioComponent* HeartbeatAudioComponent = nullptr;

	// 현재 저체력 상태인지
	bool bIsLowHealth = false;

	// 임계 비율
	float LowHealthThreshold = 0.2f;

};
