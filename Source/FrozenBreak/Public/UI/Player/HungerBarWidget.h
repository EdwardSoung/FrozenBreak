// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HungerBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UHungerBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetHungerProgress(float InValue);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> HungerProgress = nullptr;

private:
	// 플레이 시작 시 굶주림 초기화 값
	float InitValue = 0.0f;
};
