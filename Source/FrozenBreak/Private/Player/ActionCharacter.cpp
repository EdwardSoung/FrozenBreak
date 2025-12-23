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

#include "Interface/Interactable.h"
#include "Player/Components/InteractionComponent.h"
#include "PlayerComponents/CraftInventoryComponent.h"
#include "PlayerComponents/InventoryComponent.h"
#include "PlayerComponents/PlayerStatComponent.h"
#include "CommonComponents/CraftComponent.h"
#include "CommonComponents/HealthComponent.h"
#include "CommonComponents/StatComponent.h"

#include "Animation/AnimMontage.h"

#include "Tools/AxeActor.h"
#include "Kismet/GameplayStatics.h"


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
		Event->Character.OnEquipInventoryItem.AddDynamic(this, &AActionCharacter::HandEquip);
		Event->Character.OnPlayerDead.AddDynamic(this, &AActionCharacter::PlayDead);
	}
}


// Called when the game starts or when spawned
void AActionCharacter::BeginPlay()
{
	Super::BeginPlay();

	//입력 세팅은 로컬만
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
	}

	OnTakeAnyDamage.AddDynamic(this, &AActionCharacter::OnPlayerTakeDamage);

	//초기 세팅으로 하면 안됨...
	//if (DefaultToolsClass && GetMesh())
	//{
	//	FActorSpawnParameters Params;
	//	Params.Owner = this;
	//	Params.Instigator = this;

	//	CurrentTools = GetWorld()->SpawnActor<AToolActor>(DefaultToolsClass, Params);

	//	if (CurrentTools)
	//	{
	//		CurrentTools->AttachToComponent(
	//			GetMesh(),
	//			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	//			TEXT("WeaponSocket")
	//		);

	//		
	//		SetHeldItemType(CurrentTools->ToolType); // 내가 지금 뭘 들고 있는지 

	//		UE_LOG(LogTemp, Warning, TEXT("[Tool] Equipped: %s Type=%d"),
	//			*CurrentTools->GetName(), (int32)CurrentTools->ToolType);
	//	}
	//	else
	//	{
	//		SetHeldItemType(EItemType::None);
	//	}
	//}


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
	const FVector2D Input = Value.Get<FVector2D>();

	// 먼저 락이면 이동 자체 차단
	if (bLandingLocked)
	{
		return;
	}

	if (Input.IsNearlyZero())
	{
		return;
	}

	// 여기서 "뒤로 달리기" 막기: Input.Y < 0 이면 뒤로
	if (GetCharacterMovement())
	{
		if (bIsCrouched)
		{
			// 앉기는 기존 MaxWalkSpeedCrouched가 알아서 처리하지만
			// 혹시 스프린트 값이 남아있을 수 있으니 워크로 내려줌
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
		else
		{
			if (Input.Y < -0.1f) // 뒤로 입력
			{
				GetCharacterMovement()->MaxWalkSpeed = BackwardMaxSpeed;
			}
			else
			{
				if (bWantsSprint)
				{
					GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
				}
				else
				{
					GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
				}
			}
		}
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

void AActionCharacter::OnSprintStarted()
{
	if (bIsCrouched) // 앉아있으면 달리기 불가
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = 650.0f; //달리기 속도

	if (bLandingLocked)
	{
		return;
	}

	bWantsSprint = true;
}

void AActionCharacter::OnSprintStopped()
{
	GetCharacterMovement()->MaxWalkSpeed = 200.0f; //기본속도로 복귀 시킴

	if (bLandingLocked)
	{
		return;
	}
	
	bWantsSprint = false;
}

void AActionCharacter::OnInteration()
{
	if (bLandingLocked)
		return;

	const bool bToolStarted = OnToolActionStarted();
	if (bToolStarted)
		return;

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

	

	// 여기서부터 수확 시작
	bIsHarvesting = true;

	if (GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;

	if (!bToolYawLocked)
	{
		bSavedUseControllerRotationYaw = bUseControllerRotationYaw;
		bUseControllerRotationYaw = false;      // 몽타주가 재생중일때 캐릭터가 마우스에 안 돌아감
		bToolYawLocked = true;
	}

	PlayAnimMontage(HarvestMontage);


	
}


void AActionCharacter::EndHarvest()
{
	// 잠금 OFF
	bIsHarvesting = false;

	PendingHarvestTarget = nullptr;
	UE_LOG(LogTemp, Log, TEXT("PendingHarvestTarget = null"));
	PendingHarvestImpactPoint = FVector::ZeroVector;

	// 이동 복구
	if (GetCharacterMovement())
	{
		 GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		 GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	}
	if (bToolYawLocked)
	{
		bUseControllerRotationYaw = bSavedUseControllerRotationYaw;
		bToolYawLocked = false;
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
	UE_LOG(LogTemp, Warning, TEXT("[Pickaxe] OnPickaxeStarted ENTER"));
	UE_LOG(LogTemp, Warning, TEXT("[Pickaxe] Enter bIsMining=%d"), bIsMining);
	if (GetCharacterMovement()->IsFalling())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Pickaxe] return: Falling"));
		return;
	}

	if (bIsMining)
		return;

	

	bIsMining = true;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 0.0f; // 채굴시 움직이지 못하게 
	}
	if (!bToolYawLocked)
	{
		bSavedUseControllerRotationYaw = bUseControllerRotationYaw;
		bUseControllerRotationYaw = false;      // 캐릭터가 마우스에 안 돌아감
		bToolYawLocked = true;
	}


	float Len = PlayAnimMontage(PickaxeMontage);
	



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
		const EItemType ItemType = InItem->GetType(); // ✅ 진짜 타입은 이걸로

		CurrentTools = ItemFactory->SpawnTool(ItemType);

		if (CurrentTools)
		{
			// 타입에 따라 소켓 분기 
			const FName SocketName = (ItemType == EItemType::Knife)
				? FName(TEXT("hand_r_Knife"))
				: FName(TEXT("hand_r_Tool")); // 도끼/곡괭이 등

			CurrentTools->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				SocketName
			);

			// ToolType 말고 ItemType으로 들고 있는 타입 확정
			SetHeldItemType(ItemType);

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
	if (UEventSubSystem* Event = UEventSubSystem::Get(this))
	{
		Event->Status.OnSetHealth.Broadcast(-Damage);
	}

}

void AActionCharacter::PlayDead()
{
	if (!InvincibleTester && DeadAnimation)
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
			{
				Anim->Montage_Play(DeadAnimation);

				// 입력 차단
				if (APlayerController* PC = Cast<APlayerController>(GetController())) DisableInput(PC);
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
				);

			}
		}
	}
}

void AActionCharacter::EndMining()
{
	bIsMining = false;

	PendingMiningTarget = nullptr;
	UE_LOG(LogTemp, Log, TEXT("PendingMiningTarget = null"));
	PendingMiningImpactPoint = FVector::ZeroVector;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	}

	if (bToolYawLocked)
	{
		bUseControllerRotationYaw = bSavedUseControllerRotationYaw;
		bToolYawLocked = false;
	}
}


void AActionCharacter::SetHeldItemType(EItemType NewType) // 지금 뭐들고 있는지 
{
	CurrentHeldItemType = NewType;
	InteractionComponent->SetPlayerCurrentTool(CurrentHeldItemType);
}


bool AActionCharacter::OnToolActionStarted()
{
	if (CurrentHeldItemType == EItemType::Axe)
	{
		const bool bPrev = bIsHarvesting;
		OnHarvestStarted();
		return bPrev;
	}

	if (CurrentHeldItemType == EItemType::Pickaxe)
	{
		const bool bPrev = bIsMining;
		OnPickaxeStarted();
		return bPrev;
	}

	return false;
}

void AActionCharacter::OnToolHit() // 지금 들고있는 무기에 맞춰 행동
{
	if (CurrentHeldItemType == EItemType::Axe)
	{
		OnHarvestHit();
	}
	else if (CurrentHeldItemType == EItemType::Pickaxe)
	{
		OnPickaxeHit();
	}

	//무기 내구도 감소
	//PlayerStatComponent로 보내고 거기서 감소시키고 내구도 없으면 여기도 null로 보내기
	if (UEventSubSystem* Event = UEventSubSystem::Get(this))
	{
		Event->Character.OnEquippedItemUsed.Broadcast();
	}
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
	bIsActionHeld = true;

	// 누르면 Start부터 재생 시작
	PlayActionMontage_Start();
}

void AActionCharacter::OnActionReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("[INPUT] Released!"));
	bIsActionHeld = false;

	// 떼면 End로 빠져나감
	JumpToEndSection_IfPlaying();
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


void AActionCharacter::OnAttackStarted()
{
	bHasKnife = true;
	// 1) 나이프 들고 있을 때만 공격
	if (!bHasKnife)
	{
		return;
	}

	// 2) 애님/몽타주 체크
	UAnimInstance* Anim = nullptr;
	if (GetMesh())
	{
		Anim = GetMesh()->GetAnimInstance();
	}
	if (!Anim || !KnifeAttackMontage)
	{
		return;
	}

	// 3) 이미 공격 중이면:
	// - 콤보 윈도우 열려있으면 입력 큐만 쌓고 리턴
	// - 아니면 그냥 무시
	if (bIsAttacking)
	{
		if (bComboWindowOpen)
		{
			QueueNextComboInput();
		}
		return;
	}

	// 4) 혹시 Montage_IsPlaying으로도 한 번 더 방어
	if (Anim->Montage_IsPlaying(KnifeAttackMontage))
	{
		return;
	}

	// 5) 콤보 1타 시작
	StartAttackCombo();
}

void AActionCharacter::StartAttackCombo()
{
	UAnimInstance* Anim = nullptr;
	if (GetMesh())
	{
		Anim = GetMesh()->GetAnimInstance();
	}
	if (!Anim || !KnifeAttackMontage)
	{
		return;
	}

	bIsAttacking = true;
	bComboInputQueued = false;
	bComboWindowOpen = false;

	CurrentCombo = 1;

	// 몽타주 끝났을 때 bIsAttacking 풀기
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AActionCharacter::OnAttackMontageEnded);
	Anim->Montage_SetEndDelegate(EndDelegate, KnifeAttackMontage);

	// 섹션으로 점프해서 시작(Attack1)
	const FString SectionNameStr = ComboSectionPrefix.ToString() + FString::FromInt(CurrentCombo);
	const FName SectionName(*SectionNameStr);

	// 몽타주 재생
	PlayAnimMontage(KnifeAttackMontage);

	// 첫 섹션으로 점프 (섹션이 없으면 그냥 처음부터 재생)
	Anim->Montage_JumpToSection(SectionName, KnifeAttackMontage);
}

void AActionCharacter::QueueNextComboInput()
{
	// 이미 큐가 true면 또 눌러도 의미 없음 (한 번만 받자)
	if (bComboInputQueued)
	{
		return;
	}

	bComboInputQueued = true;
}

void AActionCharacter::TryGotoNextComboSection()
{
	// 윈도우가 열려있지 않으면 점프 금지
	if (!bComboWindowOpen)
	{
		return;
	}

	// 입력 큐가 없으면 점프 금지
	if (!bComboInputQueued)
	{
		return;
	}

	// 최대 콤보면 끝
	if (CurrentCombo >= MaxCombo)
	{
		return;
	}

	UAnimInstance* Anim = nullptr;
	if (GetMesh())
	{
		Anim = GetMesh()->GetAnimInstance();
	}
	if (!Anim || !KnifeAttackMontage)
	{
		return;
	}

	// 다음 콤보로
	CurrentCombo = CurrentCombo + 1;
	bComboInputQueued = false;

	const FString NextSectionStr = ComboSectionPrefix.ToString() + FString::FromInt(CurrentCombo);
	const FName NextSection(*NextSectionStr);

	// 실제 섹션 점프
	Anim->Montage_JumpToSection(NextSection, KnifeAttackMontage);
}

void AActionCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != KnifeAttackMontage)
	{
		return;
	}

	// 공격 상태 리셋
	bIsAttacking = false;
	bComboInputQueued = false;
	bComboWindowOpen = false;
	CurrentCombo = 0;
}

void AActionCharacter::SetComboWindowOpen(bool bOpen)
{
	bComboWindowOpen = bOpen;

	// 윈도우가 "열리는 순간" 이미 입력 큐가 쌓여있다면 바로 점프 시도도 가능
	if (bComboWindowOpen)
	{
		TryGotoNextComboSection();
	}
}

bool AActionCharacter::IsComboWindowOpen() const
{
	return bComboWindowOpen;
}

void AActionCharacter::Notify_TryComboJump()
{
	// AnimNotifyState에서 윈도우 중 Tick/End 시점에 호출할 용도
	TryGotoNextComboSection();
}