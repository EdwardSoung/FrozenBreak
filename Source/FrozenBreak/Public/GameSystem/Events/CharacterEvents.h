// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enums.h"
#include "Objects/InventoryItem.h"
#include "CharacterEvents.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTestDeletage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartCraft, float, InCraftAmount);

//Inventory
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetPickupItem, EItemType, InType, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInitInventoryUI, TArray<UInventoryItem*>, InData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddItemToInventoryUI, UInventoryItem*, InItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateInventoryItem, EItemType, InType);


USTRUCT(BlueprintType)
struct FCharacterEvents
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnTestDeletage OnTestDeletage;

	// 플레이어 사망 처리 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDead OnPlayerDead;

	// 작업력 셋팅. Craft 시작을 위한 델리게이트. 
	// 작업력과 함께 Broadcast를 날릴 시 타이머를 통해 해당 작업력 비율로 작업량이 늘기 시작(0~MAX).
	UPROPERTY(BlueprintAssignable)
	FOnStartCraft OnStartCraft;

	UPROPERTY(BlueprintCallable)
	FOnGetPickupItem OnGetPickupItem;
	FOnInitInventoryUI OnInitInventoryUI;
	FOnAddItemToInventoryUI OnAddItemToInventoryUI;
	FOnUpdateInventoryItem OnUpdateInventoryItem;
};
