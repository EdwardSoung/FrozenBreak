// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAssetBase.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	Wearable,		// 입을 수 있는 것
	Carryable,		// 손에 들 수 있는 것
	Eatable,		// 먹을 수 있는 것
	Material,		// 재료들
};

UCLASS(BlueprintType)
class FROZENBREAK_API UItemDataAssetBase : public UDataAsset
{
	GENERATED_BODY()

public:
	// 아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	// 아이템 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText Description;

	// 아이템 타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType ItemType = EItemType::None;

	// 아이템 무게
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "0"))
	int32 ItemWeight = 0;

	// 아이템 아이콘 (인벤토리)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UTexture2D> ItemIcon;

	// 추위 통을 얼마나 늘려주는지 (???)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wearable")
	int32 ColdResistance = 0;

	// 허기 회복량 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eatable")
	int32 HealAmount = 0;

	// 아이템 공격력 (공격, 나무나 돌 채집시)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carryable")
	int32 AttackPower = 0;

	// 아이템 내구도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carryable")
	int32 Durability = 0;
};
