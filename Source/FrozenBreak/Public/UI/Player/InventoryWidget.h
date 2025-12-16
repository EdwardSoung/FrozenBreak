// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enums.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	TArray<class UInventoryItem*> ItemDataList;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> InventoryList;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CloseButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> WeightText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> TrashButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> UseButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> UseButtonText;

protected:
	virtual void NativeOnInitialized() override;
private:
	UFUNCTION()
	void UpdateItemAmount(EItemType InType, int32 InAmount);
	UFUNCTION()
	void AddItem(class UInventoryItem* InItem);
	UFUNCTION()
	void InitData(TArray<class UInventoryItem*> InData);
};
