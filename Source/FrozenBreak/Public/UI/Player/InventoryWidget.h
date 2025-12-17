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
	float Weight = 0.0f;
	
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
	virtual void NativeConstruct() override;
private:
	UFUNCTION()
	void UpdateItemByType(EItemType InType);
	UFUNCTION()
	void AddItem(class UInventoryItem* InItem);
	UFUNCTION()
	void InitData(TArray<class UInventoryItem*> InData);
	UFUNCTION()
	void RemoveItem(class UInventoryItem* InItem);
	UFUNCTION()
	void TrashItem();
	UFUNCTION()
	void CloseWidget();

	void UpdateWeight();

};
