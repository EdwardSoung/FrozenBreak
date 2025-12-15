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
	Wood,
	Stone,
	Vine,		//µ¢Äð
	Axe,		//µµ³¢
	Pickaxe,	//°î±ªÀÌ
	Knife,
	Meat,
	Fruit,
	//..Ãß°¡ ÇÊ¿ä
};

UENUM(BlueprintType)
enum class EPropType : uint8
{
	None,
	Tree,
	Rock,
	//..Å»Ãâ±¸ µ¹¹«´õ±â¶û
	//ÀÛ¾÷´ë È­·Î µîµî..
};

UENUM(BlueprintType)
enum class EInteractKeyType : uint8
{
	Key_E,
	Key_F,
};