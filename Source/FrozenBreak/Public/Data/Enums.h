// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWidgetType : uint8
{
	HUD,
	Inventory,
	CharacterState,
	QuitMenu,
};


UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	Timber,		// 목재
	Stone,		// 돌멩이
	Vine,		// 덩쿨
	Lether,		// 동물 가죽
	Axe,		// 도끼
	Pickaxe,	// 곡괭이
	Knife,		// 칼
	RawMeat,	// 생고기
	CookedMeat,	// 구운 고기
	Fruit,		// 과일?
	Jaket		// 자켓
};

UENUM(BlueprintType)
enum class EPropType : uint8
{
	None,
	Tree,				// 나무
	Rock,				// 돌덩이 (길을 막는 돌덩이랑 차별점을 둘지 고민)
	Bed,				// 침대
	CraftingTable,		// 제작대	<- 애매함
	Campfire			// 모닥불	<- 동일

};

UENUM(BlueprintType)
enum class EInteractKeyType : uint8
{
	Key_E,
	Key_F,
};