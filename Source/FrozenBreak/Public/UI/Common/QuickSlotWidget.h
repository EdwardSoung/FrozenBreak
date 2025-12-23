// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UQuickSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> SlotParent;

private:
	TArray<class UQuickSlotItemWidget*> Slots;
protected:
	virtual void NativeOnInitialized() override;
};
