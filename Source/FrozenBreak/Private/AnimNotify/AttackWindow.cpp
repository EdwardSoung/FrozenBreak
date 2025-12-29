// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AttackWindow.h"
#include "Player/ActionCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UAttackWindow::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration
)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	AActionCharacter* Character = Cast<AActionCharacter>(OwnerActor);
	if (!Character)
	{
		return;
	}

	// 공격 판정 시작
	Character->BeginKnifeHitWindow();
}

void UAttackWindow::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation
)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	AActionCharacter* Character = Cast<AActionCharacter>(OwnerActor);
	if (!Character)
	{
		return;
	}

	// 공격 판정 종료
	Character->EndKnifeHitWindow();
}