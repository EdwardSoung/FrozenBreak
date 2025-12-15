// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommonComponents/HealthComponent.h"
#include "StatusComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROZENBREAK_API UStatusComponent : public UHealthComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

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
	virtual void BeginPlay() override;
	virtual void InitStatus() override;
	virtual void BindStatusSettingEvents() override;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	TSubclassOf<class UStatusDataAsset> StatusContinuousSetDataClass = nullptr;
};
