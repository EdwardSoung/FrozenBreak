// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_ComboAttack.h"
#include "Player/ActionCharacter.h"
#include "Components/SkeletalMeshComponent.h"

AActionCharacter* UAnimNotifyState_ComboAttack::GetOwnerCharacter(USkeletalMeshComponent* MeshComp) const
{
	if (!MeshComp)
	{
		return nullptr;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	return Cast<AActionCharacter>(Owner);
}

void UAnimNotifyState_ComboAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	AActionCharacter* Character = GetOwnerCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	// 콤보 입력 윈도우 열기
	Character->SetComboWindowOpen(true);
}

void UAnimNotifyState_ComboAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	// (선택) 윈도우 동안 입력 큐가 들어오면 즉시 점프 시도
	AActionCharacter* Character = GetOwnerCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	Character->Notify_TryComboJump();
}

void UAnimNotifyState_ComboAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	AActionCharacter* Character = GetOwnerCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	// 윈도우 닫기
	Character->SetComboWindowOpen(false);

	// 끝나는 순간 한 번 더 점프 시도(마지막 프레임에 눌렀을 때 보험)
	Character->Notify_TryComboJump();
}