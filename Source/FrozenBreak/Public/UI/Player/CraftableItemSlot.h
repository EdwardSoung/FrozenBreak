// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CraftableItemSlot.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UCraftableItemSlot : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CraftSelected;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CraftItemIcon;
};
