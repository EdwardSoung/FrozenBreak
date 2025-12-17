// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/HarvestEnd.h"
#include "Player/ActionCharacter.h"

void UHarvestEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	AActionCharacter* Character = Cast<AActionCharacter>(Owner);
	if (!Character)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[AnimNotify C++] HarvestEnd"));

	Character->EndHarvest();
}
