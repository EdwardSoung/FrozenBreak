// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterEvents.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTestDeletage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartCraft, float, InCraftAmount);

USTRUCT(BlueprintType)
struct FCharacterEvents
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnTestDeletage OnTestDeletage;

	// 작업력 셋팅. Craft 시작을 위한 델리게이트. 
	// 작업력과 함께 Broadcast를 날릴 시 타이머를 통해 해당 작업력 비율로 작업량이 늘기 시작(0~MAX).
	UPROPERTY(BlueprintAssignable)
	FOnStartCraft OnStartCraft;
};
