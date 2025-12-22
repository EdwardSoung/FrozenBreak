// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BedActionWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBedActionWidgetStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBedActionWidgetEnd);



/**
 *
 */
UCLASS()
class FROZENBREAK_API UBedActionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDayCountText(int32 InCount);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SleepAnimationFinished();

public:
	UPROPERTY(BlueprintAssignable)
	FOnBedActionWidgetStart OnBedActionWidgetStart;
	UPROPERTY(BlueprintAssignable)
	FOnBedActionWidgetEnd OnBedActionWidgetEnd;

protected:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> SleepAnim = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DayCountText = nullptr;

private:
	int32 DayCount = 0;
};
