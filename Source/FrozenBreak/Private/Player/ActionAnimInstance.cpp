// Fill out your copyright notice in the Description page of Project Settings.



#include "Player/ActionAnimInstance.h"

#include "Player/ActionCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UActionAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* Pawn = TryGetPawnOwner();
	if (!Pawn)
	{
		return;
	}

	OwnerCharacter = Cast<AActionCharacter>(Pawn);
	if (OwnerCharacter)
	{
		MovementComp = OwnerCharacter->GetCharacterMovement();
	}


}

void UActionAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter || !MovementComp)
	{
		return;
	}

	const FVector Velocity = OwnerCharacter->GetVelocity();
	Speed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();

	bIsFalling = MovementComp->IsFalling(); // 점프 애니메이션 

	bIsCrouching = OwnerCharacter->bIsCrouched;// 앉기 애니메이션
}
