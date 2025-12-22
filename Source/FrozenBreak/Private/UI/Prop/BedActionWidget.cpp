// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Prop/BedActionWidget.h"
#include "Components/TextBlock.h"

void UBedActionWidget::SetDayCountText(int32 InCount)
{
	DayCountText->SetText(FText::AsNumber(InCount));
}

void UBedActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 플레이어 조작 잠금 처리는 WolrdProp에서
	OnBedActionWidgetStart.Broadcast();
	PlayAnimation(SleepAnim, 0.f, 1, EUMGSequencePlayMode::Forward);

	// 애니메이션 재생 끝나면 바인드 되는 언리얼 내장 델리게이트인듯?
	FWidgetAnimationDynamicEvent AnimationFinished;
	AnimationFinished.BindDynamic(this, &UBedActionWidget::SleepAnimationFinished);
	BindToAnimationFinished(SleepAnim, AnimationFinished);

}

void UBedActionWidget::SleepAnimationFinished()
{
	// 재생 후 WorldProp::BedAction이 일어나야 한다.
	OnBedActionWidgetEnd.Broadcast();

	RemoveFromParent();
	SetVisibility(ESlateVisibility::Hidden);
}
