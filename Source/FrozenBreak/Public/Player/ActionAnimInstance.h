// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ActionAnimInstance.generated.h"

/**
 * 
 */

class AActionCharacter;
class UCharacterMovementComponent;

UCLASS()
class FROZENBREAK_API UActionAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:
	virtual void NativeInitializeAnimation()override;
	virtual void NativeUpdateAnimation(float DeltaSeconds)override;

protected:
	//캐릭터 참조
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	TObjectPtr<AActionCharacter> OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	TObjectPtr<UCharacterMovementComponent> MovementComp = nullptr;

	//애님 bp 에서 사용할 값
protected: // 점프 모션 사용
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float Speed = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	bool bIsFalling = false;
protected: // 앉기, 앉아서 걷기 모션 사용
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	bool bIsCrouching = false;

protected: // 달리기 
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	bool bIsSprinting = false;

protected: // 옆으로 걷기
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float RightAmount = 0.0f; // -1 은 왼쪽으로 가고 ~ +1은 오른쪽으로가게 됨

	UPROPERTY(BlueprintReadOnly, Category = "Movement|Strafe")
	float StrafeRunAlpha = 0.0f; // 옆 걷기 속도에 따른 애니메이션 변화

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	bool bIsStrafing = false;

protected: // 뒤로 걷기(뒤로 달리기도 추가 예정)
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float ForwardAmount = 0.0f; // -1은 후진  +1은 전진

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	bool bIsMovingBackward = false;



protected: // 에임 오프셋 (상반신만 움직이게)
	//UPROPERTY(BlueprintReadOnly, Category = "Aim")
	//float AimYaw = 0.0f;
	//UPROPERTY(BlueprintReadOnly, Category = "Aim")
	//float AimPitch = 0.0f;

};
