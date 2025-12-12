// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/ActionCharacter.h"

#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
AActionCharacter::AActionCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ===== Camera Boom =====
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// 카메라 이동 부드럽게
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 12.0f;

	// ===== Follow Camera =====
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// ===== Rotation Settings (배그식 TPS) =====
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = false;
	MoveComp->bUseControllerDesiredRotation = false;
	MoveComp->RotationRate = FRotator(0.f, 0.f, 0.f);

	// 점프 높이, 공중에서 캐릭터 제어
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.25f;

	//앉기 세팅
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

	//앉아 있을때 걸음 속도
	MoveComp->MaxWalkSpeedCrouched = 200.0f;
}


// Called when the game starts or when spawned
void AActionCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 플레이어만 입력 처리
	if (!IsLocallyControlled())
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!Subsystem)
	{
		return;
	}

	if (IMC_Player)
	{
		Subsystem->AddMappingContext(IMC_Player, 0);
	}

	PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = false;

	// 카메라 Pitch 제한
	if (PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->ViewPitchMin = -60.f;
		PC->PlayerCameraManager->ViewPitchMax = 60.f;
	}
}


// Called every frame
void AActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Called to bind functionality to input
void AActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput =
		Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EnhancedInput)
	{
		return;
	}

	if (IA_Move)
	{
		EnhancedInput->BindAction(
			IA_Move,
			ETriggerEvent::Triggered,
			this,
			&AActionCharacter::OnMove
		);
	}

	if (IA_Look)
	{
		EnhancedInput->BindAction(
			IA_Look,
			ETriggerEvent::Triggered,
			this,
			&AActionCharacter::OnLook
		);
	}
	if (IA_Jump)
	{
		EnhancedInput->BindAction(
			IA_Jump,
			ETriggerEvent::Started, 
			this, 
			&AActionCharacter::OnJumpStarted
		);
		EnhancedInput->BindAction(
			IA_Jump,
			ETriggerEvent::Completed, 
			this, 
			&AActionCharacter::OnJumpStopped);
	}
	if (IA_Crouch_Hold)
	{
		EnhancedInput->BindAction(
			IA_Crouch_Hold,
			ETriggerEvent::Started,
			this,
			&AActionCharacter::OnCrouchHoldStarted
		);

		EnhancedInput->BindAction(
			IA_Crouch_Hold,
			ETriggerEvent::Completed,
			this,
			&AActionCharacter::OnCrouchHoldCompleted);
	}
	if (IA_Crouch_Toggle)
	{
		EnhancedInput->BindAction(
			IA_Crouch_Toggle,
			ETriggerEvent::Started,
			this,
			&AActionCharacter::OnCrouchToggle
		);
	}
}


// ===== Movement =====
void AActionCharacter::OnMove(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	if (Input.IsNearlyZero())
	{
		return;
	}

	AddMovementInput(GetActorForwardVector(), Input.Y);
	AddMovementInput(GetActorRightVector(), Input.X);
}


// ===== Look =====
void AActionCharacter::OnLook(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	if (!Controller)
	{
		return;
	}

	AddControllerYawInput(Input.X);
	AddControllerPitchInput(Input.Y);
}

void AActionCharacter::OnJumpStarted()
{
	Jump();
}

void AActionCharacter::OnJumpStopped()
{
	StopJumping();
}

void AActionCharacter::OnCrouchHoldStarted()
{
	Crouch();
}

void AActionCharacter::OnCrouchHoldCompleted()
{
	UnCrouch();
}

void AActionCharacter::OnCrouchToggle()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}
