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
	Vine,		//����
	Axe,		//����
	Pickaxe,	//���
	Knife,
	Meat,
	Fruit,
	//..�߰� �ʿ�
};

UENUM(BlueprintType)
enum class EPropType : uint8
{
	None,
	Tree,
	Rock,
	//..Ż�ⱸ ���������
	//�۾��� ȭ�� ���..
};

UENUM(BlueprintType)
enum class EInteractKeyType : uint8
{
	Key_E,
	Key_F,
};