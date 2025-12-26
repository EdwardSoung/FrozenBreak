// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_HitEnd.h"
#include "Player/ActionCharacter.h"
#include "Components/SkeletalMeshComponent.h"



void UAnimNotify_HitEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

	Super::Notify(MeshComp, Animation);

	if (!MeshComp)
		return;
	AActionCharacter* Character = Cast<AActionCharacter>(MeshComp->GetOwner());
	if (!Character)
		return;

	//Character->EndHitReact();
}
