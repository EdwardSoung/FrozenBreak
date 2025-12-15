// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataTable.generated.h"

// 아이템 데이터 테이블 베이스

//UENUM(BlueprintType)
//enum class EItemType : uint8
//{
//	None,
//	Wearable,		// 입을 수 있는 것
//	Carryable,		// 손에 들 수 있는 것
//	Eatable,		// 먹을 수 있는 것
//	Material		// 재료로 사용 가능한 것
//};


USTRUCT(BlueprintType)
struct FROZENBREAK_API FItemDataTable : public FTableRowBase
{
	GENERATED_BODY()

	//// 아이템 이름
	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//FText DisplayName;
	//
	//// 아이템 설명
	//// meta = (MultiLine = true) : 여러줄 텍스트
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	//FText Description;
	//
	//// 아이템 타입
	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//EItemType ItemType = EItemType::None;
	//
	//// 아이템 무게
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"))
	//int32 ItemWeight = 0;
	//
	//// 인벤토리 아이콘
	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//TSoftObjectPtr<UTexture2D> ItemIcon;
	//

};
