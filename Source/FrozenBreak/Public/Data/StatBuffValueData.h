// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StatBuffValueData.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UStatBuffValueData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float TemperatureBuffValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float FatigueBuffValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float HungerBuffValue = 1.0f;
};
