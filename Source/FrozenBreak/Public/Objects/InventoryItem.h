// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/ItemData.h"
#include "InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UInventoryItem : public UObject
{
	GENERATED_BODY()
public:
	void Initialize(uint32 InUID, UItemData* InData, float InDurability = 0.0f);
	inline UItemData* GetData() { return Data.Get(); }
	inline float GetWeight() { return Data.Get()->ItemWeight * Amount; }
	inline int32 GetAmount() { return Amount; }
	inline EItemType GetType() { return Data.Get()->ItemType; }
	inline uint32 GetUID() { return UID; }
	inline float GetDurability() { return Durability; }

	void AddAmount(int32 InAmount);	
	void SetAmount(int32 InAmount);
	void ItemUsed();
private:
	uint32 UID = 0;

	TObjectPtr<UItemData> Data;
	int32 Amount = 0;
	
	float Durability = 0.0f;
	float MaxDurability = 0.0f;

public:
	int32 QuickSlotNum = 0;
};
