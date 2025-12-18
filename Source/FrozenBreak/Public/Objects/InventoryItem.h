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
	void Initialize(UItemData* InData);
	inline UItemData* GetData() { return Data.Get(); }
	inline float GetWeight() { return Data->ItemWeight * Amount; }
	inline int32 GetAmount() { return Amount; }

	void AddAmount(int32 InAmount);	
	void SetAmount(int32 InAmount);
private:
	TObjectPtr<UItemData> Data;
	int32 Amount = 0;
	
	float Durability = 0.0f;
	float MaxDurability = 0.0f;
};
