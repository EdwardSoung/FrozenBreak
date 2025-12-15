// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StatusDataAsset.generated.h"

/**
 * 스테이터스의 지속 증감 값 데이터 애셋
 * 지속적으로 증감시킬 값과 증감 타이머의 rate 값을 가지고 있다.
 */
UCLASS(BlueprintType)
class FROZENBREAK_API UStatusDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 체온 지속 증감 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float Temperature = -0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float TemperatureTimerRate = 0.1f;

	// 활동력 지속 증감 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float Fatigue = -0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float FatigueTimerRate = 0.1f;
	
	// 포만감 지속 증감 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float Hunger = -0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float HungerTimerRate = 0.1f;
};
