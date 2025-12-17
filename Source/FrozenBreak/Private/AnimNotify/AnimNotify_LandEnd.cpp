// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_LandEnd.h"
#include "Player/ActionCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotify_LandEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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

	Character->bLandingLocked = false;

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}
