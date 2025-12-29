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

#include "GameSystem/ItemFactorySubSystem.h"
#include "GameSystem/EventSubSystem.h"
#include "GameSystem/FrozenForestGameState.h"

#include "Interface/Interactable.h"
#include "Player/Components/InteractionComponent.h"
#include "PlayerComponents/CraftInventoryComponent.h"
#include "PlayerComponents/InventoryComponent.h"
#include "PlayerComponents/PlayerStatComponent.h"
#include "CommonComponents/CraftComponent.h"
#include "CommonComponents/HealthComponent.h"
#include "CommonComponents/StatComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "Tools/AxeActor.h"
#include "Kismet/GameplayStatics.h"

#include "Data/PropData.h"


// Sets default values
AActionCharacter::AActionCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ===== Camera Boom =====
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->bUsePawnControlRotation = true;

	CameraBoom->bDoCollisionTest = false;


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

	// Interaction Component
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Common Stat"));
	PlayerStatComponent = CreateDefaultSubobject<UPlayerStatComponent>(TEXT("Player Stat"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	CraftComponent = CreateDefaultSubobject<UCraftComponent>(TEXT("Crafting"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	CraftInventoryComponent = CreateDefaultSubobject<UCraftInventoryComponent>(TEXT("Craft Inventory"));


	// 점프 높이, 공중에서 캐릭터 제어
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.25f;

	//앉기 세팅
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

	//앉아 있을때 걸음 속도
	MoveComp->MaxWalkSpeedCrouched = 150.0f;

	//걷기 세팅
	MoveComp->MaxWalkSpeed = 200.0f;

	if (UEventSubSystem* Event = UEventSubSystem::Get(this))
	{
		Event->Character.OnEquipHandItem.AddDynamic(this, &AActionCharacter::HandEquip);
		Event->Character.OnPlayerDead.AddDynamic(this, &AActionCharacter::PlayDead);
	}

}



// Called when the game starts or when spawned
void AActionCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ===== 기본 초기값(무게/락 관련) =====
	WeightSpeedMultiplier = 1.0f;
	bOverweightBlocked = false;
	bMoveLockedByHarvest = false; 
	

	// ===== 입력 세팅은 로컬만 =====
	if (IsLocallyControlled())
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
			if (LocalPlayer)
			{
				UEnhancedInputLocalPlayerSubsystem* Subsystem =
					LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

				if (Subsystem && IMC_Player)
				{
					Subsystem->AddMappingContext(IMC_Player, 0);
				}
			}

			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;

			if (PC->PlayerCameraManager)
			{
				PC->PlayerCameraManager->ViewPitchMin = -45.0f;
				PC->PlayerCameraManager->ViewPitchMax = 45.0f;
			}
		}

		// ===== 인벤 무게 이벤트 바인딩 =====
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnUpdateInventoryWeight.AddDynamic(
				this,
				&AActionCharacter::OnInventoryWeightUpdated
			);
		}

		// ===== 시작 시 이동 상태 정리(안전) =====
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			// 혹시라도 MOVE_None로 시작하는 상황 방지
			if (MoveComp->MovementMode == MOVE_None)
			{
				MoveComp->SetMovementMode(MOVE_Walking);
			}

			// 시작 속도는 Recalc 구조가 처리 (입력 없으니 (0,0))
			RecalcMoveSpeed(FVector2D::ZeroVector);
		}
	}

	// 데미지 바인딩
	OnTakeAnyDamage.AddDynamic(this, &AActionCharacter::OnPlayerTakeDamage);

	TargetArmLength = CameraBoom ? CameraBoom->TargetArmLength : OutdoorArmLength;
	TargetFOV = FollowCamera ? FollowCamera->FieldOfView : OutdoorFOV;
}


// Called every frame
void AActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraBoom)
	{
		const float NewArm = FMath::FInterpTo(
			CameraBoom->TargetArmLength,
			TargetArmLength,
			DeltaTime,
			CameraZoomInterpSpeed
		);

		CameraBoom->TargetArmLength = NewArm;
	}

	if (FollowCamera)
	{
		const float NewFOV = FMath::FInterpTo(
			FollowCamera->FieldOfView,
			TargetFOV,
			DeltaTime,
			CameraZoomInterpSpeed
		);

		FollowCamera->SetFieldOfView(NewFOV);
	}
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

	if (IA_Sprint)
	{
		EnhancedInput->BindAction(
			IA_Sprint,
			ETriggerEvent::Started,
			this,
			&AActionCharacter::OnSprintStarted
		);
	}
	if (IA_Sprint)
	{
		EnhancedInput->BindAction(
			IA_Sprint,
			ETriggerEvent::Completed,
			this,
			&AActionCharacter::OnSprintStopped
		);
	}
	if (IA_Interaction)
	{
		EnhancedInput->BindAction(
			IA_Interaction,
			ETriggerEvent::Started,
			this,
			&AActionCharacter::OnInteration
		);
	}
	if (IA_Defense)
	{
		EnhancedInput->BindAction(
			IA_Defense, 
			ETriggerEvent::Started, 
			this, 
			&AActionCharacter::OnActionPressed);
	}
	if (IA_Defense)
	{
		EnhancedInput->BindAction(
			IA_Defense,
			ETriggerEvent::Completed,
			this,
			&AActionCharacter::OnActionReleased);
	}
	if (IA_Attack)
	{
		EnhancedInput->BindAction(
			IA_Attack, 
			ETriggerEvent::Started, 
			this, 
			&AActionCharacter::OnAttackStarted);
	}
	
}

void AActionCharacter::SetIndoorCameraZoom(bool bIndoor)
{
	if (bIndoor)
	{
		TargetArmLength = IndoorArmLength;
		TargetFOV = IndoorFOV;
	}
	else
	{
		TargetArmLength = OutdoorArmLength;
		TargetFOV = OutdoorFOV;
	}
}


void AActionCharacter::Landed(const FHitResult& Hit) // 착지
{
	Super::Landed(Hit);
	if (!GetCharacterMovement())
	{
		return;
	}

	const float LandingVelocityZ = GetCharacterMovement()->Velocity.Z;

	if (LandingVelocityZ <= HardLandVelocityZ && HardLandMontage)
	{
		//이동 잠금
		bLandingLocked = true;
		// 이동 완전 차단
		GetCharacterMovement()->DisableMovement();
		// 몽타주 재생
		PlayAnimMontage(HardLandMontage);
	}
}

// ===== Movement =====
void AActionCharacter::OnMove(const FInputActionValue& Value)
{
	if (bIsDead)
	{
		return;
	}
	const FVector2D Input = Value.Get<FVector2D>();

	// 완전 차단 조건들
	if (bLandingLocked || bOverweightBlocked || bMoveLockedByMining || bMoveLockedByHarvest)
	{
		return;
	}

	if (Input.IsNearlyZero())
	{
		return;
	}

	// 여기서부터는 “이동 입력”만 처리하고,
	// 속도(MaxWalkSpeed)는 RecalcMoveSpeed 한 군데에서만 최종 결정한다.
	RecalcMoveSpeed(Input);

	AddMovementInput(GetActorForwardVector(), Input.Y);
	AddMovementInput(GetActorRightVector(), Input.X);
}


void AActionCharacter::RecalcMoveSpeed(const FVector2D& LastMoveInput)
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	// 착지/과적/툴락이면 그냥 완전 차단
	if (bLandingLocked || bOverweightBlocked || bMoveLockedByMining || bMoveLockedByHarvest)
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement(); // MOVE_None
		return;
	}

	// 움직일 수 있는 상태라면 Walking 보장
	if (MoveComp->MovementMode == MOVE_None)
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	// 기본 속도 결정(걷기/스프린트/뒤로/앉기)
	float BaseSpeed = WalkSpeed;

	if (bIsCrouched)
	{
		BaseSpeed = MoveComp->MaxWalkSpeedCrouched; // 150 세팅해둔 값
	}
	else
	{
		// 뒤로 이동 제한
		if (LastMoveInput.Y < -0.1f)
		{
			BaseSpeed = BackwardMaxSpeed; // 200
		}
		else
		{
			// 스프린트
			if (bWantsSprint)
			{
				BaseSpeed = SprintSpeed; // 650
			}
			else
			{
				BaseSpeed = WalkSpeed; // 200
			}
		}
	}

	// 인벤 무게 배율 적용
	float FinalSpeed = BaseSpeed * WeightSpeedMultiplier;

	// 안전장치
	if (FinalSpeed < 0.0f)
	{
		FinalSpeed = 0.0f;
	}

	// 과적시 달리기 금지 
	if (bWantsSprint && !bSprintBlockedByWeight)
	{
		BaseSpeed = SprintSpeed;
	}
	else
	{
		BaseSpeed = WalkSpeed;
	}

	MoveComp->MaxWalkSpeed = FinalSpeed;
}

void AActionCharacter::OnInventoryWeightUpdated(float InWeight, float InMaxWeight)
{
	
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	if (InMaxWeight <= 0.0f)
	{
		// 비정상이면 페널티 해제
		WeightSpeedMultiplier = 1.0f;
		bOverweightBlocked = false;

		// 현재 입력 상태 기반으로 속도 재계산 (입력값이 없으니 (0,0) 넣음)
		RecalcMoveSpeed(FVector2D::ZeroVector);
		return;
	}

	float Ratio = InWeight / InMaxWeight;
	Ratio = FMath::Clamp(Ratio, 0.0f, 10.0f);

	// Block: 과적 -> 이동 완전 차단
	if (Ratio >= BlockThreshold)
	{
		bOverweightBlocked = true;
		WeightSpeedMultiplier = 0.0f;

		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement(); // MOVE_None

		UE_LOG(LogTemp, Warning, TEXT("[WeightUpdated] BLOCK Ratio=%.2f  W=%.1f/%.1f"), Ratio, InWeight, InMaxWeight);
		return;
	}

	// 여기까지 왔으면 Block 해제 상태
	if (bOverweightBlocked)
	{
		bOverweightBlocked = false;

		// 다시 걷기 모드 복구
		if (MoveComp->MovementMode == MOVE_None)
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}
	}

	// Slow: 감속 배율 적용
	if (Ratio >= SlowThreshold)
	{
		WeightSpeedMultiplier = SlowSpeedMultiplier; // 예: 0.5
		UE_LOG(LogTemp, Warning, TEXT("[WeightUpdated] SLOW Ratio=%.2f Mul=%.2f  W=%.1f/%.1f"),
			Ratio, WeightSpeedMultiplier, InWeight, InMaxWeight);
		bSprintBlockedByWeight = true;
		if (bWantsSprint)
		{
			bWantsSprint = false;

		}
	}
	else
	{
		WeightSpeedMultiplier = 1.0f;
		UE_LOG(LogTemp, Warning, TEXT("[WeightUpdated] NORMAL Ratio=%.2f Mul=%.2f  W=%.1f/%.1f"),
			Ratio, WeightSpeedMultiplier, InWeight, InMaxWeight);
		bSprintBlockedByWeight = false;
	}

	// 최종 속도 재계산 (현재 입력 벡터를 알 수 없으니 0 넣고,
	// 실제 이동 중이면 다음 OnMove에서 Recalc가 다시 한 번 들어가면서 정확히 갱신됨)
	RecalcMoveSpeed(FVector2D::ZeroVector);
	
}


// ===== Look =====
void AActionCharacter::OnLook(const FInputActionValue& Value)
{
	if (bIsDead)
	{
		return;
	}
	const FVector2D Input = Value.Get<FVector2D>();
	if (!Controller)
	{
		return;
	}

	AddControllerYawInput(Input.X);
	AddControllerPitchInput(Input.Y);
}
void AActionCharacter::PlayJumpSFX()
{
	if (bIsDead)
	{
		return;
	}
	if (bHitLocked) return;
	// 무음 확률
	if (JumpSkipChance > 0.0f && FMath::FRand() < JumpSkipChance)
	{
		return;
	}

	const int32 Count = JumpSounds.Num();
	if (Count <= 0)
	{
		return;
	}

	int32 PickedIndex = FMath::RandRange(0, Count - 1);

	if (bAvoidSameJumpSound && Count > 1)
	{
		while (PickedIndex == LastJumpSoundIndex)
		{
			PickedIndex = FMath::RandRange(0, Count - 1);
		}
	}

	LastJumpSoundIndex = PickedIndex;

	USoundBase* SoundToPlay = JumpSounds[PickedIndex];
	if (!SoundToPlay)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(
		this,
		SoundToPlay,
		GetActorLocation(),
		VolumeMultiplier,
		PitchMultiplier
	);
}

void AActionCharacter::OnJumpStarted()
{
	if (bIsDead)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[Jump] Jump() CALLED"));
	if(GetCharacterMovement()->IsFalling())
	{
		return;
	}
	PlayJumpSFX();
	Super::Jump();
}

void AActionCharacter::OnJumpStopped()
{
	if (bIsDead)
	{
		return;
	}
	StopJumping();
}

void AActionCharacter::OnCrouchHoldStarted()
{
	if (bIsDead)
	{
		return;
	}
	Crouch();
}

void AActionCharacter::OnCrouchHoldCompleted()
{
	if (bIsDead)
	{
		return;
	}
	UnCrouch();
}

void AActionCharacter::OnCrouchToggle()
{
	if (bIsDead)
	{
		return;
	}
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AActionCharacter::OnSprintStarted()
{
	if (bIsDead)
	{
		return;
	}
	if (bHitLocked) 
		return;
	if (bIsCrouched)
		return;

	if (bLandingLocked || bOverweightBlocked || bMoveLockedByMining || bMoveLockedByHarvest)
		return;
	if (bSprintBlockedByWeight)
		return;

	bWantsSprint = true;

	
	RecalcMoveSpeed(FVector2D(0.0f, 1.0f));
}

void AActionCharacter::OnSprintStopped()
{
	if (bIsDead)
	{
		return;
	}
	bWantsSprint = false;

	if (bLandingLocked || bOverweightBlocked || bMoveLockedByMining || bMoveLockedByHarvest)
		return;

	RecalcMoveSpeed(FVector2D(0.0f, 1.0f));
}

void AActionCharacter::OnInteration()
{
	if (bIsDead)
	{
		return;
	}

	if (bHitLocked) 
		return;
	if (bLandingLocked)
		return;

	// 플레이어가 PendingHarvest(Mining)Target을 직접 설정할떄의 코드
	// 반대되는 코드는 Interaction Component -> DoAction()에 있다.
	if (InteractionComponent)
	{
		if (AActor* FocusedActor = InteractionComponent->GetCurrentInteractionActor())
		{
			const EItemType ToolType = InteractionComponent->GetCurrentActorInteractableToolType();

			if (FocusedActor)
			{
				if (CurrentHeldItemType == EItemType::Axe && ToolType == EItemType::Axe)
				{
					SetPendingHarvestTarget(FocusedActor);
				}
				if (CurrentHeldItemType == EItemType::Pickaxe && ToolType == EItemType::Pickaxe)
				{
					SetPendingMiningTarget(FocusedActor);
				}
			}
		}
	}

	if (bIsMining || bIsHarvesting)
		return;

	OnToolActionStarted();

	IInteractable::Execute_DoAction(InteractionComponent);
}

void AActionCharacter::OnHarvestStarted()
{
	/*
	UE_LOG(LogTemp, Warning, TEXT("[HarvestStart] Enter Held=%d bIsHarvesting=%d Falling=%d Tools=%s"),
		(int32)CurrentHeldItemType,
		bIsHarvesting,
		GetCharacterMovement() ? GetCharacterMovement()->IsFalling() : -1,
		CurrentTools ? *CurrentTools->GetName() : TEXT("None")
	);
	*/

	if (GetCharacterMovement()->IsFalling())
	{
		
		//UE_LOG(LogTemp, Warning, TEXT("Blocked: Falling"));
		return;

	}

	if (bIsHarvesting)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Blocked: Already Harvesting"));
		return;
	}
	if (!HarvestMontage)
	{
		return;
	}
	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;

	if (!Anim)
	{
		
		return;
	}
	
	if (Anim->Montage_IsPlaying(HarvestMontage))
	{
		
		return;
	}

	// 여기서부터 수확 시작
	bIsHarvesting = true;

	if (GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;

	if (!bToolYawLocked)
	{
		SaveToolLockSnapshot();
		bUseControllerRotationYaw = false;      // 몽타주가 재생중일때 캐릭터가 마우스에 안 돌아감
		bToolYawLocked = true;
	}
	FOnMontageEnded EndDel;
	EndDel.BindUObject(this, &AActionCharacter::OnToolMontageEnded);
	Anim->Montage_SetEndDelegate(EndDel, HarvestMontage);

	// 몽타주 재생
	Anim->Montage_Play(HarvestMontage);
}


void AActionCharacter::EndHarvest()
{
	bIsHarvesting = false;
	bMoveLockedByHarvest = false;

	PendingHarvestTarget = nullptr;
	PendingHarvestImpactPoint = FVector::ZeroVector;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		if (MoveComp->MovementMode == MOVE_None)
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}

		// 무게/스프린트 상태 반영해서 최종 속도 갱신
		RecalcMoveSpeed(FVector2D::ZeroVector);
	}

	if (bToolYawLocked)
	{
		RestoreToolLockSnapshot();
	}
}

void AActionCharacter::OnHarvestHit()
{
	if (!bIsHarvesting) return;

	AActor* Target = PendingHarvestTarget.Get();
	UE_LOG(LogTemp, Log, TEXT("PendingHarvestTarget 설정됨"));
	if (!Target) return;

	if (!Target->ActorHasTag("Tree"))
		return;

	if (Target->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		if (AxeHitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, AxeHitSound, PendingHarvestImpactPoint);
		}
		IInteractable::Execute_DoAction(Target);
	}
}



void AActionCharacter::OnPickaxeStarted() // 곡괭이 전용
{
	
	// 공중이면 채굴 금지
	if (GetCharacterMovement() && GetCharacterMovement()->IsFalling())
	{
		return;
	}

	// 필수 체크
	if (!PickaxeMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Mining] PickaxeMontage is NULL"));
		return;
	}

	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!Anim)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Mining] AnimInstance is NULL"));
		return;
	}

	// 이미 이 몽타주 재생 중이면 연타 무시
	if (Anim->Montage_IsPlaying(PickaxeMontage))
	{
		UE_LOG(LogTemp, Verbose, TEXT("[Mining] PickaxeMontage already playing -> ignore"));
		return;
	}

	// 상태 플래그로도 한번 더 방어
	if (bIsMining)
	{
		return;
	}

	// 여기서부터 "채굴 시작" 확정
	bIsMining = true;

	// 이동 잠금 (원래 속도 저장)
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		SavedWalkSpeed = MoveComp->MaxWalkSpeed;
		MoveComp->MaxWalkSpeed = 0.0f;
		bMoveLockedByMining = true;
	}

	// Yaw 잠금(마우스로 캐릭터 회전 금지)
	if (!bToolYawLocked)
	{
		SaveToolLockSnapshot();
		bUseControllerRotationYaw = false;
		bToolYawLocked = true;
	}

	// EndDelegate는 "이번 재생 1회"에만 걸리게 세팅
	FOnMontageEnded EndDel;
	EndDel.BindUObject(this, &AActionCharacter::OnToolMontageEnded);
	Anim->Montage_SetEndDelegate(EndDel, PickaxeMontage);

	// 몽타주 재생
	Anim->Montage_Play(PickaxeMontage);

}


void AActionCharacter::OnPickaxeHit()
{

	if (!bIsMining)
	{
		return;
	}
	if (CurrentHeldItemType != EItemType::Pickaxe) // 안전장치 나중에  도구 바꿀때 쓸꺼
	{
		return;
	}

	AActor* Target = PendingMiningTarget.Get();
	UE_LOG(LogTemp, Log, TEXT("PendingMiningTarget 설정됨"));
	if (!Target)
	{
		return;
	}

	if (Target->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		if (PickaxeHitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickaxeHitSound, PendingMiningImpactPoint);
		}

		//테스트용 주석처리 쌉가능
		IInteractable::Execute_DoAction(Target);
	}
}

void AActionCharacter::HandEquip(UInventoryItem* InItem)
{
	if (CurrentTools)
	{
		// 도구가 내구도가 닳아 없어지면 End~~~() 실행 (End~~~() 안되면 카메라가 안 돌아온다)
		if (bIsHarvesting) { EndHarvest(); }
		if (bIsMining) { EndMining(); }

		CurrentTools->Destroy();
		CurrentTools = nullptr;
		SetHeldItemType(EItemType::None);
	}

	if (!InItem)
	{
		return;
	}	

	if (UItemFactorySubSystem* ItemFactory = UItemFactorySubSystem::Get(this))
	{
		const EItemType ItemType = InItem->GetType(); //진짜 타입은 이걸로

		CurrentTools = ItemFactory->SpawnTool(ItemType, InItem->GetDurability());

		if (CurrentTools)
		{
			// 타입에 따라 소켓 분기 
			const FName SocketName = (ItemType == EItemType::Knife)
				? FName(TEXT("hand_r_Knife"))
				: FName(TEXT("hand_r_Tool")); // 도끼/곡괭이 등
			UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(CurrentTools->GetRootComponent());
			if (Prim)
			{
				Prim->SetSimulatePhysics(false);
				Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

			CurrentTools->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				SocketName
			);
			CurrentTools->SetActorRelativeLocation(FVector::ZeroVector);
			CurrentTools->SetActorRelativeRotation(FRotator::ZeroRotator);
			UE_LOG(LogTemp, Warning, TEXT("[Attach] Socket=%s Exists=%d"),
				*SocketName.ToString(),
				GetMesh() ? (GetMesh()->DoesSocketExist(SocketName) ? 1 : 0) : 0);

			UE_LOG(LogTemp, Warning, TEXT("[Attach] WorldLoc=%s WorldRot=%s"),
				*CurrentTools->GetActorLocation().ToString(),
				*CurrentTools->GetActorRotation().ToString());

			UE_LOG(LogTemp, Warning, TEXT("[Attach] Parent=%s"),
				CurrentTools->GetAttachParentActor() ? *CurrentTools->GetAttachParentActor()->GetName() : TEXT("None"));
			UE_LOG(LogTemp, Warning, TEXT("[Attach] Mesh=%s Skeleton=%s"),
				*GetNameSafe(GetMesh()),
				GetMesh() && GetMesh()->GetSkeletalMeshAsset() ? *GetNameSafe(GetMesh()->GetSkeletalMeshAsset()->GetSkeleton()) : TEXT("NULL"));


			// ToolType 말고 ItemType으로 들고 있는 타입 확정
			SetHeldItemType(ItemType);
			bHasKnife = (ItemType == EItemType::Knife);
			UE_LOG(LogTemp, Warning, TEXT("[EquipState] bHasKnife=%d Held=%s"),
				bHasKnife, *UEnum::GetValueAsString(ItemType));

			UE_LOG(LogTemp, Warning, TEXT("[Tool] Equipped: %s ItemType=%s Socket=%s ToolType=%s"),
				*CurrentTools->GetName(),
				*UEnum::GetValueAsString(ItemType),
				*SocketName.ToString(),
				*UEnum::GetValueAsString(CurrentTools->ToolType));
		}
		else
		{
			SetHeldItemType(EItemType::None);
		}
	}
}

void AActionCharacter::OnPlayerTakeDamage(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser)
{
	if (auto GameState = GetWorld()->GetGameState<AFrozenForestGameState>())
	{
		if (GameState->GetGameState() != EGameState::Playing)
		{
			return;
		}
	}

	if (UEventSubSystem* Event = UEventSubSystem::Get(this))
	{
		Event->Status.OnSetHealth.Broadcast(-Damage);
	}
	PlayHitReaction();
}

void AActionCharacter::PlayDead()
{
	if (bIsDead || InvincibleTester)
	{
		return;
	}

	if (DeadAnimation)
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
			{				
				Anim->Montage_Play(DeadAnimation);

				bIsDead = true;
				// 입력 차단
				/*if (APlayerController* PC = Cast<APlayerController>(GetController())) DisableInput(PC);
				FTimerHandle DeadTimerHandle;
				GetWorldTimerManager().SetTimer(
					DeadTimerHandle,
					[this]() {
						UGameplayStatics::OpenLevel(
							this,
							FName(TEXT("FrozenBreak"))
						);
					},
					2.5f,
					false
				);*/

			}
		}
	}
}

void AActionCharacter::EndMining()
{
	bIsMining = false;
	bMoveLockedByMining = false;

	PendingMiningTarget = nullptr;
	PendingMiningImpactPoint = FVector::ZeroVector;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		if (MoveComp->MovementMode == MOVE_None)
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}

		RecalcMoveSpeed(FVector2D::ZeroVector);
	}

	if (bToolYawLocked)
	{
		RestoreToolLockSnapshot();
	}

}

void AActionCharacter::SetHeldItemType(EItemType NewType) // 지금 뭐들고 있는지 
{
	CurrentHeldItemType = NewType;
	InteractionComponent->SetPlayerCurrentTool(CurrentHeldItemType);
}


bool AActionCharacter::OnToolActionStarted()
{
	if (CurrentHeldItemType == EItemType::Axe && PendingHarvestTarget.IsValid())
	{
		const bool bPrev = bIsHarvesting;
		OnHarvestStarted();
		return bPrev;
	}

	if (CurrentHeldItemType == EItemType::Pickaxe && PendingMiningTarget.IsValid())
	{
		const bool bPrev = bIsMining;
		OnPickaxeStarted();
		return bPrev;
	}

	return false;
}

void AActionCharacter::SaveToolLockSnapshot()
{
	if (bToolYawLocked)
	{
		return;
	}

	bSavedUseControllerRotationYaw = bUseControllerRotationYaw;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		bSaveOrientRotationToMovement = MoveComp->bOrientRotationToMovement;
	}
}

void AActionCharacter::RestoreToolLockSnapshot()
{
	if (!bToolYawLocked)
	{
		return;
	}

	// 회전 설정 복구
	bUseControllerRotationYaw = bSavedUseControllerRotationYaw;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = bSaveOrientRotationToMovement;
	}

	bToolYawLocked = false;
}

void AActionCharacter::OnToolMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HarvestMontage)
	{
		EndHarvest();                // 기존 정리 로직(이동/플래그) 유지
		RestoreToolLockSnapshot();   
	}
	else if (Montage == PickaxeMontage)
	{
		EndMining();
		RestoreToolLockSnapshot();
	}
}

void AActionCharacter::OnDeadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
		{
			Anim->Montage_Pause(Montage);
		}
	}	
}

void AActionCharacter::OnToolHit() // 지금 들고있는 무기에 맞춰 행동
{
	if (CurrentHeldItemType == EItemType::Axe)
	{
		OnHarvestHit();
		if (UEventSubSystem* Event = UEventSubSystem::Get(this))
		{
			Event->Character.OnEquipHandItemUsed.Broadcast();
		}
	}
	else if (CurrentHeldItemType == EItemType::Pickaxe)
	{
		OnPickaxeHit();
		if (UEventSubSystem* Event = UEventSubSystem::Get(this))
		{
			Event->Character.OnEquipHandItemUsed.Broadcast();
		}
	}

	//무기 내구도 감소
	//PlayerStatComponent로 보내고 거기서 감소시키고 내구도 없으면 여기도 null로 보내기
	
}

void AActionCharacter::OnToolEnd()
{
	if (CurrentHeldItemType == EItemType::Axe)
	{
		EndHarvest();
	}
	else if (CurrentHeldItemType == EItemType::Pickaxe)
	{
		EndMining();
	}
}

void AActionCharacter::OnActionPressed()
{

	UE_LOG(LogTemp, Warning, TEXT("[INPUT] Pressed!"));
	//bIsActionHeld = true;

	// 누르면 Start부터 재생 시작
	//PlayActionMontage_Start();
}

void AActionCharacter::OnActionReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("[INPUT] Released!"));
	//bIsActionHeld = false;

	// 떼면 End로 빠져나감
	//JumpToEndSection_IfPlaying();
}

UAnimInstance* AActionCharacter::GetMyAnimInstance() const
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return nullptr;
	}

	return MeshComp->GetAnimInstance();
}

void AActionCharacter::PlayActionMontage_Start()
{
	if (!ActionMontage)
	{
		return;
	}

	UAnimInstance* Anim = GetMyAnimInstance();
	if (!Anim)
	{
		return;
	}

	// 이미 재생중이면 중복 방지
	if (Anim->Montage_IsPlaying(ActionMontage))
	{
		return;
	}

	// 재생
	Anim->Montage_Play(ActionMontage, 1.0f);

	// Start 섹션으로 시작
	Anim->Montage_JumpToSection(SectionStart, ActionMontage);
}

void AActionCharacter::JumpToEndSection_IfPlaying()
{
	if (!ActionMontage)
	{
		return;
	}

	UAnimInstance* Anim = GetMyAnimInstance();
	if (!Anim)
	{
		return;
	}

	if (!Anim->Montage_IsPlaying(ActionMontage))
	{
		return;
	}
	

	// End 섹션으로 점프해서 루프 탈출
	Anim->Montage_JumpToSection(SectionEnd, ActionMontage);
}


void AActionCharacter::PlayHitReaction()
{
	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;

	// 여기서 return 하면 VFX/SFX까지 막혀서 제거
	// if (Anim && Anim->IsAnyMontagePlaying())
	// {
	//     return;
	// }

	// 위치 계산: 소켓 있으면 소켓 위치, 없으면 액터 위치
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp && !BloodSocketName.IsNone() && MeshComp->DoesSocketExist(BloodSocketName))
	{
		SpawnLocation = MeshComp->GetSocketLocation(BloodSocketName);
		SpawnRotation = MeshComp->GetSocketRotation(BloodSocketName);
	}

	// VFX는 항상 재생 
	if (BloodVFXList.Num() > 0)
	{
		int32 PickedIndex = FMath::RandRange(0, BloodVFXList.Num() - 1);

		if (bAvoidSameBloodVFX && BloodVFXList.Num() > 1)
		{
			while (PickedIndex == LastBloodVFXIndex)
			{
				PickedIndex = FMath::RandRange(0, BloodVFXList.Num() - 1);
			}
		}

		LastBloodVFXIndex = PickedIndex;

		UNiagaraSystem* PickedVFX = BloodVFXList[PickedIndex];
		if (PickedVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				PickedVFX,
				SpawnLocation,
				SpawnRotation
			);
		}
	}

	// SFX도 항상 재생 
	if (HitSoundList.Num() > 0)
	{
		int32 PickedIndex = FMath::RandRange(0, HitSoundList.Num() - 1);

		if (bAvoidSameHitSound && HitSoundList.Num() > 1)
		{
			while (PickedIndex == LastHitSoundIndex)
			{
				PickedIndex = FMath::RandRange(0, HitSoundList.Num() - 1);
			}
		}

		LastHitSoundIndex = PickedIndex;

		USoundBase* PickedSFX = HitSoundList[PickedIndex];
		if (PickedSFX)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				PickedSFX,
				SpawnLocation,
				HitVolumeMultiplier,
				HitPitchMultiplier
			);
		}
	}

	// 여기부터는 "히트 몽타주"만 가능하면 재생 

	// 애님 인스턴스 없으면 몽타주는 못 틀지만, VFX/SFX는 이미 나갔으니 종료
	if (!Anim || HitMontages.Num() <= 0)
	{
		return;
	}

	// 다른 몽타주가 돌고 있으면 "히트 몽타주만" 스킵
	if (Anim->IsAnyMontagePlaying())
	{
		return;
	}

	int32 PickedIndex = FMath::RandRange(0, HitMontages.Num() - 1);

	if (bAvoidSameHitMontage && HitMontages.Num() > 1)
	{
		while (PickedIndex == LastHitMontageIndex)
		{
			PickedIndex = FMath::RandRange(0, HitMontages.Num() - 1);
		}
	}
	LastHitMontageIndex = PickedIndex;

	UAnimMontage* PickedMontage = HitMontages[PickedIndex];
	if (!PickedMontage)
	{
		return;
	}

	if (Anim->Montage_IsPlaying(PickedMontage))
	{
		return;
	}

	// 히트 액션 시작 → 이동 금지
	bHitLocked = true;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	// delegate 안정화: Play 먼저, 성공하면 EndDelegate
	const float PlayedLen = Anim->Montage_Play(PickedMontage);
	if (PlayedLen <= 0.0f)
	{
		bHitLocked = false;

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			if (!bOverweightBlocked && !bLandingLocked && !bMoveLockedByMining && !bMoveLockedByHarvest)
			{
				MoveComp->SetMovementMode(MOVE_Walking);
			}
			else
			{
				MoveComp->DisableMovement();
			}
		}

		RecalcMoveSpeed(FVector2D::ZeroVector);
		return;
	}

	FOnMontageEnded EndDel;
	EndDel.BindUObject(this, &AActionCharacter::OnHitMontageEnded);
	Anim->Montage_SetEndDelegate(EndDel, PickedMontage);

}

void AActionCharacter::OnHitMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bHitLocked = false;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		// 과적/착지락 같은 다른 “진짜 락”이 없으면 걷기 복구
		if (!bOverweightBlocked && !bLandingLocked && !bMoveLockedByMining && !bMoveLockedByHarvest)
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}
		else
		{
			// 여전히 막혀야 하는 상황이면 계속 막기
			MoveComp->DisableMovement();
		}
	}

	// 무게/스프린트/뒤로 제한 포함해서 최종 속도 다시 계산
	RecalcMoveSpeed(FVector2D::ZeroVector);
}

void AActionCharacter::OnAttackStarted()
{
	if (bHitLocked)
	{
		return;
	}

	if (!bHasKnife)
	{
		return;
	}

	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!Anim || !KnifeAttackMontage)
	{
		return;
	}

	// 공격 중이면: 몽타주 재생 중이면 콤보 예약만
	if (bIsAttacking)
	{
		if (!Anim->Montage_IsPlaying(KnifeAttackMontage))
		{
			// 상태 꼬였으면 리셋
			bIsAttacking = false;
			bComboRequested = false;
			CurrentCombo = 0;
		}
		else
		{
			bComboRequested = true;
			return;
		}
	}

	StartAttackCombo();
}



void AActionCharacter::StartAttackCombo()
{
	
	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!Anim || !KnifeAttackMontage)
		return;

	bIsAttacking = true;
	bComboRequested = false;
	CurrentCombo = 1;

	FOnMontageEnded EndDel;
	EndDel.BindUObject(this, &AActionCharacter::OnAttackMontageEnded);
	Anim->Montage_SetEndDelegate(EndDel, KnifeAttackMontage);

	Anim->Montage_Play(KnifeAttackMontage);
	Anim->Montage_JumpToSection(TEXT("Attack1"), KnifeAttackMontage);
}


void AActionCharacter::Notify_TryComboJump()
{
	if (!bComboRequested)
	{
		return;
	}

	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!Anim || !KnifeAttackMontage)
	{
		return;
	}

	if (CurrentCombo >= MaxCombo)
	{
		return;
	}

	bComboRequested = false;
	CurrentCombo = CurrentCombo + 1;

	const FName NextSection = FName(*FString::Printf(TEXT("Attack%d"), CurrentCombo));
	Anim->Montage_JumpToSection(NextSection, KnifeAttackMontage);
}

void AActionCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != KnifeAttackMontage)
	{
		return;
	}
	EndKnifeHitWindow();

	bIsAttacking = false;
	bComboRequested = false;
	CurrentCombo = 0;
}
void AActionCharacter::BeginKnifeHitWindow()
{
	if (!bIsAttacking)
	{
		return;
	}

	// 이미 열려있으면 중복 방지
	if (bKnifeHitWindowOpen)
	{
		return;
	}

	bKnifeHitWindowOpen = true;
	KnifeHitActors.Empty();

	// 주기적으로 트레이스
	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(
			KnifeTraceTimerHandle,
			this,
			&AActionCharacter::TickKnifeHitWindow,
			KnifeTraceInterval,
			true
		);
	}
}

void AActionCharacter::EndKnifeHitWindow()
{
	if (!bKnifeHitWindowOpen)
	{
		return;
	}

	bKnifeHitWindowOpen = false;
	KnifeHitActors.Empty();

	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(KnifeTraceTimerHandle);
	}
}

void AActionCharacter::TickKnifeHitWindow()
{
	if (!bKnifeHitWindowOpen)
	{
		return;
	}

	// 혹시 공격이 끝났는데 타이머만 남아있으면 정리
	if (!bIsAttacking)
	{
		EndKnifeHitWindow();
		return;
	}

	// 캐릭터 기준 앞쪽 SphereTraceMulti
	const FVector Forward = GetActorForwardVector();
	const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, KnifeTraceUp) + Forward * KnifeTraceForwardOffset;
	const FVector End = Start + Forward * KnifeTraceDistance;

	TArray<FHitResult> Hits;

	// 트레이스 무시 목록
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	const bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		this,
		Start,
		End,
		KnifeTraceRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hits,
		true
	);

	if (!bHit)
	{
		return;
	}

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			continue;
		}

		// 나 자신 제외
		if (HitActor == this)
		{
			continue;
		}

		// “한 윈도우 내 중복 타격 방지”
		TWeakObjectPtr<AActor> Key(HitActor);
		if (KnifeHitActors.Contains(Key))
		{
			continue;
		}

		KnifeHitActors.Add(Key);

		// 데미지 적용
		UGameplayStatics::ApplyDamage(
			HitActor,
			KnifeDamage,
			GetController(),
			this,
			UDamageType::StaticClass()
		);
	}
}

