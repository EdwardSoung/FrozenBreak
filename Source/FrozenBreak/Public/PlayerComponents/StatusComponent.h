// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROZENBREAK_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

	// 온도 증감 함수
	void SetPlayerTemperature(float InTemperatureValue);

	// 체력 증감 함수
	void SetPlayerHealth(float InHealthValue);

protected:
	virtual void BeginPlay() override;

private:
	// 게임 시작 시 각 Current 값들을 Max 값으로 초기화
	void InitStatus();

public:

protected:
	// ------------------------------------------------------------------------------ 플레이어 체력 값
	// 플레이어 최대 체력 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Temperature")
	float MaxHealth = 100.0f;

	// 플레이어 현재 체력 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Temperature")
	float CurrentHealth = 0.0f;
	// ------------------------------------------------------------------------------|

	// ------------------------------------------------------------------------------ 플레이어 온도 값
	// 플레이어 최대 온도 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Temperature")
	float MaxTemperature = 100.0f;

	// 플레이어 현재 온도 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Temperature")
	float CurrentTemperature = 0.0f;
	// ------------------------------------------------------------------------------|

private:
	// ------------------------------------------------------------------------------ Default 변수
	// 각 수치의 최소값
	float DefaultMinimum = 0.0f;
	// ------------------------------------------------------------------------------|	
};
