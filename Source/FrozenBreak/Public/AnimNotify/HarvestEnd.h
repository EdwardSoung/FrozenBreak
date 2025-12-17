// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HarvestEnd.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UHarvestEnd : public UAnimNotify
{
	GENERATED_BODY()


public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
