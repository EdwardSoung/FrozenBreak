// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enums.h"
#include "ItemObjectTableData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FROZENBREAK_API FItemObjectTableData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemType Type;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class APickupItem> ItemClass;
};
