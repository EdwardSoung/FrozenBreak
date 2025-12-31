// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AnimalAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API AAnimalAIControllerBase : public AAIController
{
	GENERATED_BODY()
	
protected:
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animal|Controller")
	TObjectPtr <class UBehaviorTree> BehaviorTreeAsset = nullptr;
};
