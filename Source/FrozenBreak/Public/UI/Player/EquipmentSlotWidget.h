// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void RefreshSlot(UTexture2D* InIcon, FText InName);

protected:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ItemIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemName = nullptr;

private:

};
