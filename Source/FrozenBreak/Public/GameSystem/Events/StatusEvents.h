// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatusEvents.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthPointChanged, float, InCurrentHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTemperaturePointChanged, float, InCurrentTemperature);

USTRUCT(BlueprintType)
struct FStatusEvents
{
	GENERATED_BODY()

	// 플레이어 체력
	UPROPERTY(BlueprintAssignable)
	FOnHealthPointChanged OnHealthPointChanged;

	// 플레이어 체온
	UPROPERTY(BlueprintAssignable)
	FOnTemperaturePointChanged OnTemperaturePointChanged;
};