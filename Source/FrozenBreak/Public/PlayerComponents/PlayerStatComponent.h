// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROZENBREAK_API UPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerStatComponent();
	// 온도 증감 함수
	UFUNCTION(BlueprintCallable, Category = "Status|Temperature")
	void SetPlayerTemperature(float InTemperatureValue);

	// 피로도 증감 함수
	UFUNCTION(BlueprintCallable, Category = "Status|Stamina")
	void SetPlayerFatigue(float InFatigueValue);

	// 포만감 증감 함수
	UFUNCTION(BlueprintCallable, Category = "Status|Hunger")
	void SetPlayerHunger(float InHungerValue);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void InitStatus();
	void BindStatSettingEvents();

public:

protected:
	// ------------------------------------------------------------------------------ 플레이어 온도 값
	// 플레이어 최대 온도 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Temperature")
	float MaxTemperature = 100.0f;

	// 플레이어 현재 온도 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Temperature")
	float CurrentTemperature = 0.0f;
	// ------------------------------------------------------------------------------|


	// ------------------------------------------------------------------------------ 플레이어 피로도 값
	// 플레이어 최대 온도 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Fatigue")
	float MaxFatigue = 100.0f;

	// 플레이어 현재 온도 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Fatigue")
	float CurrentFatigue = 0.0f;
	// ------------------------------------------------------------------------------|


	// ------------------------------------------------------------------------------ 플레이어 포만감 값
	// 플레이어 최대 온도 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Hunger")
	float MaxHunger = 100.0f;

	// 플레이어 현재 온도 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Hunger")
	float CurrentHunger = 0.0f;
	// ------------------------------------------------------------------------------|
};
