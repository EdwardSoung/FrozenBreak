// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enums.h"
#include "Objects/InventoryItem.h"
#include "CharacterEvents.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTestDeletage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartCraft, float, InCraftAmount); // 캐릭터가 실제 작업을 시작하는 델리게이트

//Inventory
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetPickupItem, EItemType, InType, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInitInventoryUI, TArray<UInventoryItem*>, InData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddItemToInventoryUI, UInventoryItem*, InItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateInventoryItem, EItemType, InType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestInventoryInit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrashItem, UInventoryItem*, InItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSendInventoryData, TArray<UInventoryItem*>, InData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateInventoryWeight, float, InWeight, float, InMaxWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUseInventoryItem, UInventoryItem*, InData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipInventoryItem, UInventoryItem*, InData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquippedItemUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUsableItemUsed, UItemData*, InType);


//Craft
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestIventoryItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInitCraftUI, TArray<UInventoryItem*>, InData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInitCraftInventoryUI, TArray<UInventoryItem*>, InData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddItemToCraftInventoryUI, UInventoryItem*, InItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateCraftItem, EItemType, InType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveCraftItem, UInventoryItem*, InItem);

//Cook
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestIventoryRawMeet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSendRawMeet, TArray<UInventoryItem*>, InData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftRequested, UInventoryItem*, InItem); // 제작 버튼을 누를 시 날리는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCookRequested, UInventoryItem*, InItem); // 요리 버튼을 누를 시 날리는 델리게이트

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
	FOnRequestInventoryInit OnRequestInventoryInit;
	FOnTrashItem OnTrashItem;
	FOnSendInventoryData OnSendInventoryData;
	FOnUpdateInventoryWeight OnUpdateInventoryWeight;
	FOnUseInventoryItem OnUseInventoryItem;
	FOnEquipInventoryItem OnEquipInventoryItem;
	FOnEquippedItemUsed OnEquippedItemUsed;
	FOnUsableItemUsed OnUsableItemUsed;

	UPROPERTY(BlueprintAssignable)
	FOnInitCraftUI OnInitCraftUI;

	UPROPERTY(BlueprintAssignable)
	FOnAddItemToCraftInventoryUI OnAddItemToCraftInventoryUI;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateCraftItem OnUpdateCraftItem;

	UPROPERTY(BlueprintAssignable)
	FOnRemoveCraftItem OnRemoveCraftItem;

	UPROPERTY(BlueprintAssignable)
	FOnRequestIventoryItems OnRequestIventoryItems;

	UPROPERTY(BlueprintAssignable)
	FOnRequestIventoryRawMeet OnRequestIventoryRawMeet;
	
	UPROPERTY(BlueprintAssignable)
	FOnSendRawMeet OnSendRawMeet;

	UPROPERTY(BlueprintAssignable)
	FOnCraftRequested OnCraftRequested;

	UPROPERTY(BlueprintAssignable)
	FOnCookRequested OnCookRequested;
};
