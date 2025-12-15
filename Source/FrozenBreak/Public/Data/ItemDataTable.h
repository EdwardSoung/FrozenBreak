// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataTable.generated.h"

// 아이템 데이터 테이블 베이스

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	Wearable,
	Carryable,
	Eatable,
	Material
};


USTRUCT(BlueprintType)
struct FROZENBREAK_API FItemDataTable : public FTableRowBase
{
	GENERATED_BODY()

	// 아이템 이름
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText DisplayName;
	
	// 아이템 설명
	// meta = (MultiLine = true) : 여러줄 텍스트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText Description;

	// 아이템 타입
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EItemType ItemType = EItemType::None;

	// 아이템 무게
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemWeight = 0;

};
