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

	AActionCharacter* Character = GetOwnerCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	// 윈도우 열려 있을 때만 점프 시도 (충돌 방지)
	if (Character->IsComboWindowOpen())
	{
		Character->Notify_TryComboJump();
	}
}

void UAnimNotifyState_ComboAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	AActionCharacter* Character = GetOwnerCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	// 끝나는 순간 한 번 더 점프 시도(보험) - 닫기 전에 마지막으로
	if (Character->IsComboWindowOpen())
	{
		Character->Notify_TryComboJump();
	}

	// 윈도우 닫기
	Character->SetComboWindowOpen(false);
}