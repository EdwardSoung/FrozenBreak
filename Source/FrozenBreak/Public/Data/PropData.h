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
	FText DisplayName;		//Ȥ�� ��ü �̸� ǥ������ �;� ����

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	EItemType GenerateItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	int32 GenerateItemCount;		//�����̸� ���������� �����ʿ�
};
