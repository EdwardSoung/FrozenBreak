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
	EItemType InteractableToolType; // 이 프롭과 상호 작용 가능한 도구 (도끼, 곡괭이. 나무와 바위 용)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	int32 GenerateItemCount;		//랜덤이면 범위값으로 수정필요

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	int32 CurrentSpawnCount;		// 나무, 바위가 GenerateItemCount을 생성한 횟수

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	int32 MaxSpawnCount;			// 나무, 바위가 GenerateItemCount을 생성할 수 있는 최대 수

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	EInteractKeyType InteractionKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	float Durability;
};
