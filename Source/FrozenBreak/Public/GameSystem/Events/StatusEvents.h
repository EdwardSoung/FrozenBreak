// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatusEvents.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthPointChanged, float, InCurrentHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTemperaturePointChanged, float, InCurrentTemperature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFatiguePointChanged, float, InCurrentFatigue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHungerPointChanged, float, InCurrentHunger);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInGameTimeChanged, FText, InTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTemperatureDefenceChanged, FText, InDefenceValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandEquipmentChanged, FText, InHandItem); // 현재 Item type이 정해지지 않아 Text로 임시 구현
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBodyEquipmentChanged, FText, InBodyItem); // 현재 Item type이 정해지지 않아 Text로 임시 구현

USTRUCT(BlueprintType)
struct FStatusEvents
{
	GENERATED_BODY()

	// 체력
	UPROPERTY(BlueprintAssignable)
	FOnHealthPointChanged OnHealthPointChanged;

	// 플레이어 체온
	UPROPERTY(BlueprintAssignable)
	FOnTemperaturePointChanged OnTemperaturePointChanged;

	// 플레이어 피로도
	UPROPERTY(BlueprintAssignable)
	FOnFatiguePointChanged OnFatiguePointChanged;

	// 플레이어 포만감
	UPROPERTY(BlueprintAssignable)
	FOnHungerPointChanged OnHungerPointChanged;

	UPROPERTY(BlueprintAssignable)
	FOnInGameTimeChanged OnInGameTimeChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnTemperatureDefenceChanged OnTemperatureDefenceChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHandEquipmentChanged OnHandEquipmentChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnBodyEquipmentChanged OnBodyEquipmentChanged;
};