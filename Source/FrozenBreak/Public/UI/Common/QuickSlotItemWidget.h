// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UQuickSlotItemWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ItemIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SlotNumberText;

protected:
	virtual void NativeOnInitialized() override;
public:
	void SetSlotNumber(int32 InNumber);
	void SetIcon(class UTexture2D* InTexture);
};
