// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Prop/BedActionWidget.h"

void UBedActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 자는 애니메이션 이니까 플레이어에게 플레이어 조작을 막으라고 해야한다.
	OnBedActionWidgetStart.Broadcast();
	PlayAnimation(SleepAnim, 0.f, 1, EUMGSequencePlayMode::Forward);

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
