// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enums.h"
#include "Objects/InventoryItem.h"
#include "UIEvents.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnToggleQuitMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemSelected, EItemType, InType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetItemToQuickSlot, int32, InIndex, UInventoryItem* InItem);


USTRUCT(BlueprintType)
struct FROZENBREAK_API FUIEvents
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnToggleQuitMenu OnToggleQuitMenu;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryItemSelected OnInventoryItemSelected;
	FOnSetItemToQuickSlot OnSetItemToQuickSlot;
};
