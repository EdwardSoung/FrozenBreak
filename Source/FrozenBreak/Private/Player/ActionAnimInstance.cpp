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
	const FVector2D Vel2D(Velocity.X, Velocity.Y);
	Speed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();

	const float AccelSize2D = MovementComp->GetCurrentAcceleration().Size2D();
	const bool hasInput = (AccelSize2D > 5.0f);

	bIsFalling = MovementComp->IsFalling(); // 점프 애니메이션 

	bIsCrouching = OwnerCharacter->bIsCrouched;// 앉기 애니메이션

	bIsSprinting = (Speed > 520.0f) && !bIsCrouching;

	if (Vel2D.SizeSquared() < 1.0f)
	{
		RightAmount = 0.0f;
		bIsStrafing = false;

		ForwardAmount = 0.0f;
		bIsMovingBackward = false;

		return;
	}
	else
	{
		//월드 속도를 캐릭터 로컬로 변환시켜둠
		const FRotator ActorRot = OwnerCharacter->GetActorRotation();
		const FVector LocalVel = ActorRot.UnrotateVector(Velocity);


		// 좌우 성분 정규화
		const FVector2D Local2D(LocalVel.X, LocalVel.Y);
		const float Len = Local2D.Size();

		if (Len > 1.0f)
		{
			//y 가 오른쪽은 + 왼쪽은 -
			RightAmount = FMath::Clamp(Local2D.Y / Len, -1.0f, 1.0f);
			bIsStrafing = (FMath::Abs(RightAmount) >= 0.35f && (Speed >= 10.0f));

			// x가 앞은 + 뒤는 -
			ForwardAmount = FMath::Clamp(Local2D.X/ Len, -1.0f, 1.0f);
			bIsMovingBackward = (ForwardAmount <= -0.35f) && (Speed >= 10.0f);
		}
		else
		{
			RightAmount = 0.0f;
			bIsStrafing = false;

			ForwardAmount = 0.0f;
			bIsMovingBackward = false;

		}
		if (bIsStrafing) // 옆걷기 속도에 따라 애니메이션 변화
		{
			// 원하는 값으로 조절 가능
			const float WalkSpeed = 150.0f;
			const float RunSpeed = 520.0f;

			StrafeRunAlpha = FMath::Clamp((Speed - WalkSpeed) / (RunSpeed - WalkSpeed), 0.0f, 1.0f);
		}
		else
		{
			StrafeRunAlpha = 0.0f;
		}

		
	}
	/*
	if (OwnerCharacter)
	{
		AController* Ctrl = OwnerCharacter->GetController();
		if (Ctrl)
		{
			const FRotator ControlRot = Ctrl->GetControlRotation();
			const FRotator ActorRot = OwnerCharacter->GetActorRotation();

			// Yaw: 시점 - 캐릭터 (정규화)
			const float RawYaw = ControlRot.Yaw - ActorRot.Yaw;
			AimYaw = FRotator::NormalizeAxis(RawYaw);

			// Pitch: 컨트롤러 Pitch를 -90~90로 정규화 
			float P = ControlRot.Pitch;
			P = FRotator::NormalizeAxis(P);
			AimPitch = P;
		}
	}
	*/
}



