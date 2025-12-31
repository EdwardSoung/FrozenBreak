// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/AI/AnimalAIControllerBase.h"
#include "BehaviorTree/BehaviorTree.h"

void AAnimalAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset) RunBehaviorTree(BehaviorTreeAsset);
}
