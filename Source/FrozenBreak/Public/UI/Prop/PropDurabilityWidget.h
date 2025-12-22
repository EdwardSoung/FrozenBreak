// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PropDurabilityWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UPropDurabilityWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void SetDurabilityProgress(float InValue);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> DurabilityProgress = nullptr;
};
