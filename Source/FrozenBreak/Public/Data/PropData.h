// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/Enums.h"
#include "PropData.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UPropData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	EPropType PropType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	FText DisplayName;		//혹시 물체 이름 표기할지 싶어 세팅

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	EItemType GenerateItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	int32 GenerateItemCount;		//랜덤이면 범위값으로 수정필요

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	float Durability;
};
