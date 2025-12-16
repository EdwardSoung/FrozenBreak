// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enums.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	//우선은 맵으로... 데이터로 할지 고민 필요
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> InteractionType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> InteractionKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<class UInteractionTextData> TextData;

public:
	void UpdateInteraction(EPropType InType, EInteractKeyType InKey);
	void UpdateInteraction(EItemType InType, EInteractKeyType InKey);
};
