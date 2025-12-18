// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/Enums.h"
#include "CraftingRecipeRow.generated.h"

USTRUCT(BlueprintType)
struct FRecipeIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemType ItemType = EItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FROZENBREAK_API FCraftingRecipeRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERecipeCategory Category = ERecipeCategory::Crafting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemType ResultItemType = EItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 ResultCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FRecipeIngredient> Required;
};
