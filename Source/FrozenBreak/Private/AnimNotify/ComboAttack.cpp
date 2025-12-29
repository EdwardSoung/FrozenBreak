// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/ComboAttack.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Player/ActionCharacter.h"

void UComboAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp) return;

	if (AActionCharacter* Char = Cast<AActionCharacter>(MeshComp->GetOwner()))
	{
		Char->SetComboWindowOpen(true);
	}
}

void UComboAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
		if (!MeshComp) return;

		if (AActionCharacter* Char = Cast<AActionCharacter>(MeshComp->GetOwner()))
		{
			Char->SetComboWindowOpen(false);
		}
}
