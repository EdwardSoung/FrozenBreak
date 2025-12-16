// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatusEvents.generated.h"

// UI 갱신 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthPointChanged, float, InCurrentHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTemperaturePointChanged, float, InCurrentTemperature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFatiguePointChanged, float, InCurrentFatigue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHungerPointChanged, float, InCurrentHunger);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInGameTimeChanged, FText, InTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTemperatureDefenceChanged, FText, InDefenceValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandEquipmentChanged, FText, InHandItem); // 현재 Item type이 정해지지 않아 Text로 임시 구현
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBodyEquipmentChanged, FText, InBodyItem); // 현재 Item type이 정해지지 않아 Text로 임시 구현
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentCraftCostChanged, float, InCurrentCraftCost);

// 스테이터스 값 세팅 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetHealth, float, InHealthValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetTemperature, float, InTemperatureValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetFatigue, float, InFatigueValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetHunger, float, InHungerValue);

USTRUCT(BlueprintType)
struct FStatusEvents
{
	GENERATED_BODY()

	// 체력 변화
	UPROPERTY(BlueprintAssignable)
	FOnHealthPointChanged OnHealthPointChanged;

	// 플레이어 체온 변화
	UPROPERTY(BlueprintAssignable)
	FOnTemperaturePointChanged OnTemperaturePointChanged;

	// 플레이어 피로도
	UPROPERTY(BlueprintAssignable)
	FOnFatiguePointChanged OnFatiguePointChanged;

	// 플레이어 포만감
	UPROPERTY(BlueprintAssignable)
	FOnHungerPointChanged OnHungerPointChanged;

	// 인게임 시간
	UPROPERTY(BlueprintAssignable)
	FOnInGameTimeChanged OnInGameTimeChanged;

	// 플레이어 방한도
	UPROPERTY(BlueprintAssignable)
	FOnTemperatureDefenceChanged OnTemperatureDefenceChanged;

	// 플레이어 손 장비
	UPROPERTY(BlueprintAssignable)
	FOnHandEquipmentChanged OnHandEquipmentChanged;
	
	// 플레이어 몸통 장비
	UPROPERTY(BlueprintAssignable)
	FOnBodyEquipmentChanged OnBodyEquipmentChanged;

	// 체력 셋팅
	UPROPERTY(BlueprintAssignable)
	FOnSetHealth OnSetHealth;

	// 플레이어 온도 셋팅
	UPROPERTY(BlueprintAssignable)
	FOnSetTemperature OnSetTemperature;

	// 플레이어 활동력 셋팅
	UPROPERTY(BlueprintAssignable)
	FOnSetFatigue OnSetFatigue;
	
	// 플레이어 포만감 셋팅
	UPROPERTY(BlueprintAssignable)
	FOnSetHunger OnSetHunger;

	// 작업량 변화량
	UPROPERTY(BlueprintAssignable)
	FOnCurrentCraftCostChanged OnCurrentCraftCostChanged;
};