// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TemperatureBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UTemperatureBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetTemperatureProgress(float InValue);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> TemperatureProgress = nullptr;

private:
	// 플레이 시작 시 체온 초기화 값
	float InitValue = 1.0f;
};
