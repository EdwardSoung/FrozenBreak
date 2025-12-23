// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotItemWidget.generated.h"

class UInventoryItem;
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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemAmountText;

private:
	int32 SlotNumber = -1;
protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void ItemUsed(UInventoryItem* InItem);

	UFUNCTION()
	void SetItem(int32 InSlotNum, UInventoryItem* InItem);
public:
	void SetSlotNumber(int32 InNumber);
	void SetSlotData(UInventoryItem* InItem);
};
