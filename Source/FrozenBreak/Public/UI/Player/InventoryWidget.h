// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enums.h"
#include "Interface/UI/Openable.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UInventoryWidget : public UUserWidget, public IOpenable
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
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
private:
	UFUNCTION()
	void UpdateItemByType(EItemType InType);
	UFUNCTION()
	void AddItem(class UInventoryItem* InItem);
	UFUNCTION()
	void InitData(TArray<class UInventoryItem*> InData);
	UFUNCTION()
	void UpdateItem(class UInventoryItem* InItem);
	UFUNCTION()
	void DropItem();
	UFUNCTION()
	void UseItem();
	UFUNCTION()
	void HideWidget();
	UFUNCTION()
	void UpdateWeight(float InWeight, float InMaxWeight);
	UFUNCTION()
	void SelectionChanged(EItemType InType);

	UFUNCTION()
	void ResetQuickSlotNumber(uint32 UID);

public:
	void OpenWidget_Implementation();
	void CloseWidget_Implementation();
};
