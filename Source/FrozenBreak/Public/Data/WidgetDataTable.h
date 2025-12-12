// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/Enums.h"
#include "WidgetDataTable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FROZENBREAK_API FWidgetDataTable : public FTableRowBase
{
	 GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWidgetType Type;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UUserWidget> WidgetClass;
};
