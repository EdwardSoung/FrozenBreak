// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Interactable.h"
#include "Engine/EngineTypes.h" 
#include "Sound/SoundBase.h"
#include "Tools/AxeActor.h"
#include "Animation/AnimMontage.h"
#include "Objects/InventoryItem.h"

#include "ActionCharacter.generated.h"

// ===== Forward Declarations =====
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
class UAnimMontage;
class UNiagaraSystem;
class USoundBase;

struct FInputActionValue;

USTRUCT(BlueprintType)
struct FFootstepSoundList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<USoundBase>> Sounds;
};

UCLASS()
class FROZENBREAK_API AActionCharacter : public ACharacter, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values
	AActionCharacter();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	inline float GetCurrentToolAtkPower() const { return CurrentTools ? CurrentTools->GetToolAtkPower() : 0.0f; }
	inline AActor* GetCurrentTool() const { return CurrentTools; }

	inline void SetPendingHarvestTarget(AActor* InTarget) { PendingHarvestTarget = InTarget; }
	inline void SetPendingMiningTarget(AActor* InTarget) { PendingMiningTarget = InTarget; }
	
protected:
	// ===== Camera =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera = nullptr;

	// ===== Enhanced Input =====
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Crouch_Hold = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Crouch_Toggle = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Sprint = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Interaction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Defense = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Attack = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Player = nullptr;

	// Interaction Component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Component")
	TObjectPtr<class UInteractionComponent> InteractionComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Component")
	TObjectPtr<class UCraftComponent> CraftComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Component")
	TObjectPtr<class UHealthComponent> HealthComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Component")
	TObjectPtr<class UStatComponent> StatComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Component")
	TObjectPtr<class UCraftInventoryComponent> CraftInventoryComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Component")
	TObjectPtr<class UInventoryComponent> InventoryComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Component")
	TObjectPtr<class UPlayerStatComponent> PlayerStatComponent = nullptr;

public: //랜딩
	UPROPERTY(EditAnywhere, Category = "Landing")
	float HardLandVelocityZ = -900.0f;

	UPROPERTY(VisibleAnywhere, Category = "Landing")
	bool bLandingLocked = false;

	UPROPERTY(EditAnywhere, Category = "Landing")
	UAnimMontage* HardLandMontage;

	virtual void Landed(const FHitResult& Hit) override;

protected: // 달리기 조건 설정

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	bool bWantsSprint = false;

	// 속도 상수 
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeed = 650.0f;

	// 뒤로는 여기까지만 
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BackwardMaxSpeed = 200.0f;

private: // 맨손일때 방어 모션
	// ===== 설정할 몽타주 =====
	UPROPERTY(EditAnywhere, Category = "Action|Montage")
	TObjectPtr<UAnimMontage> ActionMontage = nullptr;

	// 섹션 이름(오타 방지용)
	UPROPERTY(EditAnywhere, Category = "Action|Montage")
	FName SectionStart = FName("Start");

	UPROPERTY(EditAnywhere, Category = "Action|Montage")
	FName SectionHold = FName("Hold");

	UPROPERTY(EditAnywhere, Category = "Action|Montage")
	FName SectionEnd = FName("End");

	// 키를 누르고 있는 상태인지
	bool bIsActionHeld = false;

	UAnimInstance* GetMyAnimInstance() const;

	// 유틸: 몽타주 실행
	void PlayActionMontage_Start();

	// 유틸: End로 탈출
	void JumpToEndSection_IfPlaying();


protected: // 도끼질

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Harvest")
	UAnimMontage* HarvestMontage = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Harvest")
	bool bIsHarvesting = false;

	UPROPERTY(EditAnywhere, Category = "Harvest|Sound")
	USoundBase* AxeHitSound = nullptr; // 도끼질 사운드

	UPROPERTY(EditAnywhere, Category = "Harvest")
	TWeakObjectPtr<AActor> PendingHarvestTarget; // 시작때 잡아둔 수확대상

	UPROPERTY(EditAnywhere, Category = "Harvest")
	FVector PendingHarvestImpactPoint = FVector::ZeroVector; // 시작때 맞춘 위치 와 사운드

protected: // 곡괭이질

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mining")
	bool bIsMining = false;

	UPROPERTY(EditAnywhere, Category = "Mining")
	TWeakObjectPtr<AActor> PendingMiningTarget;

	UPROPERTY(EditAnywhere, Category = "Mining")
	FVector PendingMiningImpactPoint = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Mining")
	UAnimMontage* PickaxeMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mining|Sound")
	USoundBase* PickaxeHitSound = nullptr;
	
	float SavedWalkSpeed = 0.0f;

	// 채굴 중 이동 잠금 걸려있는지 (안전장치)
	bool bMoveLockedByMining = false;

protected: // 아이템 관련

	UPROPERTY(EditDefaultsOnly, Category = "Tool")
	TSubclassOf<class AToolActor> DefaultToolsClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tool")
	TObjectPtr<AToolActor> CurrentTools = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetHeldItemType(EItemType NewType);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Test")
	bool InvincibleTester = true; // 테스트용 무적 플래그

protected:
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weight|Penalty")
		float BlockThreshold = 1.0f;
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weight|Penalty")
		float SlowThreshold = 0.7f;
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weight|Penalty")
		float SlowSpeedMultiplier = 0.5f;
private:
	
	// 인벤 무게로 인한 배율
	float WeightSpeedMultiplier = 1.0f;

	// 과적 상태(이동 완전 차단)
	bool bOverweightBlocked = false;

	// “툴 액션 중 이동 잠금” (도끼용도 추가하는게 좋아)
	bool bMoveLockedByHarvest = false;

	// 속도 최종 재계산(딱 한 군데에서만 MaxWalkSpeed 만짐)
	void RecalcMoveSpeed(const FVector2D& LastMoveInput);

	// 무게 패널티가 시작되면 달리기 금지.
	bool bSprintBlockedByWeight = false;

	// 무게 이벤트
	UFUNCTION()
	void OnInventoryWeightUpdated(float InWeight, float InMaxWeight);

protected:
	// ===== Input Functions =====
	void OnMove(const FInputActionValue& Value);

	void OnLook(const FInputActionValue& Value);

	void OnJumpStarted();
	void OnJumpStopped();

	void OnCrouchHoldStarted();
	void OnCrouchHoldCompleted();
	void OnCrouchToggle();

	void OnSprintStarted();
	void OnSprintStopped();

	void OnInteration();

	//도끼질
	void OnHarvestStarted();

	//곡괭이질
	void OnPickaxeStarted();

	//통합버전 

	bool OnToolActionStarted();

	bool bSavedUseControllerRotationYaw = true;
	bool bToolYawLocked = false;
	//툴 액션몽타주 실행중 버그 패치
	bool bSaveOrientRotationToMovement = false;

	void SaveToolLockSnapshot();
	void RestoreToolLockSnapshot();

	UFUNCTION()
	void OnToolMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	



	//장착
	UFUNCTION(BlueprintCallable)
	void HandEquip(UInventoryItem* InItem);

	UFUNCTION()
	void OnPlayerTakeDamage(
		AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		AController* InstigatedBy,
		AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	void PlayDead();

public:
	//곡괭이질
	void EndMining();
	void OnPickaxeHit();

	//도끼질
	void EndHarvest();
	void OnHarvestHit();

	//BP에서 바로 호출할 용도
	UFUNCTION(BlueprintCallable, Category = "ToolAction")
	void OnToolHit();
	UFUNCTION(BlueprintCallable, Category = "ToolAction")
	void OnToolEnd();

	UFUNCTION()
	void OnActionPressed(); // 맨손일때 방어키 누른상태
	UFUNCTION()
	void OnActionReleased(); // 맨손일 때 방어키 땔 때 


	


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType CurrentHeldItemType = EItemType::None;

	EItemType GetCurrentToolsType() const { return CurrentHeldItemType; }

public:
	//사운드용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TArray<USoundBase*> FootstepSounds_L; // 왼발

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TArray<USoundBase*> FootstepSounds_R; // 오른발

	//발소리 중복 방지용
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	int32 LastFootstepIndex_L = -1;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	int32 LastFootstepIndex_R = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TMap<TEnumAsByte<EPhysicalSurface>, FFootstepSoundList> FootstepBySurface_L;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TMap<TEnumAsByte<EPhysicalSurface>, FFootstepSoundList> FootstepBySurface_R;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	TEnumAsByte<EPhysicalSurface> LastFootSurface = SurfaceType_Default; // 공중에 잠깐뜰때나 계단 같은 곳에서의 설정

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dead|Montage")
	TObjectPtr<class UAnimMontage> DeadAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Jump")
	TArray<TObjectPtr<USoundBase>> JumpSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Jump", meta = (ClampMin = "0.0"))
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Jump", meta = (ClampMin = "0.0"))
	float PitchMultiplier = 1.0f;

	// 같은 소리 연속 방지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Jump")
	bool bAvoidSameJumpSound = true;

	// 점프 사운드 무음 확률 (0.0 = 항상 재생)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Jump", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float JumpSkipChance = 0.0f;

	int32 LastJumpSoundIndex = INDEX_NONE;
	void PlayJumpSFX();


	// 피격 VFX (Niagara)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit|VFX")
	TArray<TObjectPtr<UNiagaraSystem>> BloodVFXList;
	// 피격 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit|SFX")
	TArray<TObjectPtr<USoundBase>> HitSoundList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit|VFX")
	bool bAvoidSameBloodVFX = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit|SFX")
	bool bAvoidSameHitSound = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit|SFX", meta = (ClampMin = "0.0"))
	float HitVolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit|SFX", meta = (ClampMin = "0.0"))
	float HitPitchMultiplier = 1.0f;

	// 피 튀길 소켓(없으면 캡슐/액터 위치)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
	FName BloodSocketName = TEXT("spine_03");

	UFUNCTION(BlueprintCallable)
	void Debug_Hit(); // 디버그용 나중에 지우기

	//중복되는 효과 방지용 인덱스 
	int32 LastBloodVFXIndex = INDEX_NONE;
	int32 LastHitSoundIndex = INDEX_NONE;

	//피격 애니메이션 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit|Montage")
	TArray<TObjectPtr<UAnimMontage>> HitMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit|Montage")
	bool bAvoidSameHitMontage = true;


	int32 LastHitMontageIndex = INDEX_NONE;
	
public: // 무기쪽
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Montage")
	TObjectPtr<class UAnimMontage> UnarmedAttackMontage;


protected: // 공격
	// ===== Input callbacks =====
	void OnAttackStarted();


	// ===== Combo helpers =====
	void StartAttackCombo();
	bool bComboRequested = false;
	bool bComboWindowOpen = false;

	
public:

	// (AnimNotifyState에서 콤보 점프 호출)
	void Notify_TryComboJump();
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	FORCEINLINE void SetComboWindowOpen(bool bOpen) { bComboWindowOpen = bOpen; }
	FORCEINLINE bool IsComboWindowOpen() const { return bComboWindowOpen; }

private:

	// ====== Weapon state ======
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bHasKnife = false; // 네 프로젝트에서 실제 나이프 장착 상태로 갱신해줘야 함

	// ====== Attack montage ======
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Montage")
	TObjectPtr<UAnimMontage> KnifeAttackMontage;

	// ====== Attack gating ======
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	bool bIsAttacking = false;

	// ====== Combo ======
	UPROPERTY(VisibleAnywhere, Category = "Combat|Combo")
	int32 CurrentCombo = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Combo")
	int32 MaxCombo = 2;


	// 섹션 이름 규칙: Attack1, Attack2, Attack3 ...
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Combo")
	FName ComboSectionPrefix = TEXT("Attack");
};