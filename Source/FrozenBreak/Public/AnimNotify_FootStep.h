// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AnimNotify_FootStep.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EFootstepSide : uint8
{
	Left,
	Right
};

UCLASS()
class FROZENBREAK_API UAnimNotify_FootStep : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootStep")
	EFootstepSide FootstepSide = EFootstepSide::Left;

	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
	
};
