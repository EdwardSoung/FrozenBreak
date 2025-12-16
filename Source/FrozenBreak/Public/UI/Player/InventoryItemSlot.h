// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventoryItemSlot.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UInventoryItemSlot : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Selected;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Icon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AmountText;

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
};
