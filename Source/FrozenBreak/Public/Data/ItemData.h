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
	// ������ Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	// ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText Description;

	// ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "0"))
	int32 ItemWeight = 0;

	// ������ ������ (�κ��丮)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UTexture2D> ItemIcon;
};
