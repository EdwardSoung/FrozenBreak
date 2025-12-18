// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftInventroyWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UCraftInventroyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void StartCraft();

private:
	UFUNCTION()
	void UpdateCraftItemByType(EItemType InType);

	UFUNCTION()
	void RequestInventoryData();

	UFUNCTION()
	void AddCraftItem(class UInventoryItem* InItem);

	UFUNCTION()
	void InitCraftableData(TArray<class UInventoryItem*> InData);

	UFUNCTION()
	void RemoveCraftItem(UInventoryItem* InItem);

	UFUNCTION()
	void CloseCraft();

private:
	TArray<class UInventoryItem*> CraftItemDataList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> CarftInventoryList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CloseCraftButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CraftButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CraftButtonText;

};
