// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Interactable.h"
#include "Tools/AxeActor.h"
#include "ActionCharacter.generated.h"

// ===== Forward Declarations =====
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

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

protected:
	// ===== Camera =====
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
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
	TObjectPtr<UInputAction> IA_Harvest = nullptr; // 나무 캐기 전용

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Player = nullptr;

	// Interaction Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Component")
	TObjectPtr<class UInteractionComponent> InteractionComponent = nullptr;

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



protected: // 아이템 관련

	UPROPERTY(EditDefaultsOnly, Category = "Tool")
	TSubclassOf<class AToolActor> DefaultToolsClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tool")
	AToolActor* CurrentTools = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType CurrentHeldItemType = EItemType::None;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetHeldItemType(EItemType NewType);


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
	void OnHarvestStarted();

public:
	void EndHarvest();
	void OnHarvestHit();
};