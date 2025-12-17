// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AxeHit.h"
#include "Player/ActionCharacter.h"

void UAxeHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    AActionCharacter* Character = Cast<AActionCharacter>(MeshComp->GetOwner());
    if (!Character) return;

    Character->OnHarvestHit(); // 타격시만 데미지 
}
