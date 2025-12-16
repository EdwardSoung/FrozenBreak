// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterEvents.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTestDeletage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetCraftCost, float, InCraftAmount);

USTRUCT(BlueprintType)
struct FCharacterEvents
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnTestDeletage OnTestDeletage;

	//작업력에 따른 작업량 셋팅
	UPROPERTY(BlueprintAssignable)
	FOnSetCraftCost OnSetCraftCost;
};
