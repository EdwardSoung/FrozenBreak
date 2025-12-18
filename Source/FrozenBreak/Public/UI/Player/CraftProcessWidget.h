// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftProcessWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UCraftProcessWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void SetCraftProcess(float InCurrentProgress);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> CraftProcess = nullptr;

private:
	float DefaultProgress = 0.0f;
};
