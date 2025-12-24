// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/Enums.h"
#include "ItemData.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UItemData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "0"))
	float ItemWeight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> ItemIcon;

	// 아이템 내구도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Durability = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EInteractKeyType InteractionKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float CraftCost = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	bool IsStackable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Stat")
	TMap<EItemStatType, float> Stats;
};
