// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Prop/BedActionWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UBedActionWidget::SetDayCountText(int32 InCount)
{
	DayCountText->SetText(FText::AsNumber(InCount));
}

void UBedActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 플레이어 조작 잠금 처리는 WolrdProp에서
	OnBedActionWidgetStart.Broadcast();

	// 애니메이션 재생 끝나면 바인드 되는 언리얼 내장 델리게이트인듯?
	FWidgetAnimationDynamicEvent AnimationFinished;
	AnimationFinished.BindDynamic(this, &UBedActionWidget::SleepAnimationFinished);
	BindToAnimationFinished(SleepAnim, AnimationFinished);

	// 위젯 애니메이션 길이의 중간에서 시간이 지나게 하기
	float Duration = SleepAnim->GetEndTime();
	const float MidTime = FMath::Max(0.01f, Duration * 0.5f);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer
		(
			SleepMidTimerHandle,
			this,
			&UBedActionWidget::SleepAnimationMidPoint,
			MidTime,
			false
		);
	}

	PlayAnimation(SleepAnim, 0.f, 1, EUMGSequencePlayMode::Forward);

}

void UBedActionWidget::SleepAnimationMidPoint()
{
	OnBedActionWidgetMid.Broadcast();
}

void UBedActionWidget::SleepAnimationFinished()
{
	// 타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SleepMidTimerHandle);
	}

	OnBedActionWidgetEnd.Broadcast();

	RemoveFromParent();
	SetVisibility(ESlateVisibility::Hidden);
}
