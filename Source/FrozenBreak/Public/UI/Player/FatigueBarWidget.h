// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FatigueBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UFatigueBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetFatigueProgress(float InValue);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> FatigueProgress = nullptr;

private:
	// 플레이 시작 시 피로도 초기화 값
	float InitValue = 0.0f;
};
