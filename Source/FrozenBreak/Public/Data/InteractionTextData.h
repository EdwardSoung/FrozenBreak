// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/Enums.h"
#include "InteractionTextData.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UInteractionTextData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EPropType, FText> PropTypeText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EItemType, FText> ItemTypeText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EInteractKeyType, FText> KeyText;
};
