// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/TryComboJump.h"
#include "Player/ActionCharacter.h"
#include "Components/SkeletalMeshComponent.h"


void UTryComboJump::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	AActionCharacter* Character = Cast<AActionCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	Character->Notify_TryComboJump();
}