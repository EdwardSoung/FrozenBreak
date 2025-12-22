// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/Enums.h"
#include "EquipItemData.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UEquipItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Tool")
	TMap<EItemType,TSubclassOf<class AToolActor>> ToolData;
};
