// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/Enums.h"
#include "Data/ItemData.h"
#include "ItemDataList.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UItemDataList : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TMap<EItemType, TObjectPtr<UItemData>> ItemAssetData;
};
