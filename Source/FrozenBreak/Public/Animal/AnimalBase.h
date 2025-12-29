// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Damageable.h"
#include "Data/Enums.h"
#include "AnimalBase.generated.h"

UCLASS()
class FROZENBREAK_API AAnimalBase : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAnimalBase();

	inline void SetAnimalType(EAnimalType  InType) { AnimalType = InType; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnTakeDamage(
		AActor* DamagedActor,
		float Damage, 
		const UDamageType* DamageType,
		AController* InstigatedBy,
		AActor* DamageCauser);

private:
	void StartHealthBarTimer();

	void PlayHit();
	void PlayDead();

	UFUNCTION()
	void DestroyAnimal();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentHealth = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWidgetComponent> HealthBarWidgetComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UEnemyHealthBarWidget> HealthBar = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> AnimalHitAnimation = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> AnimalDeadAnimation = nullptr;

	float DefaultMinimum = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 RawMeatDropCount = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 LeatherDropCount = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float AnimalDamage = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EAnimalType AnimalType = EAnimalType::None;

private:
	FTimerHandle HealthBarTimerHandle;
	FTimerHandle DeadTimerHandle;
	float HealthBarVisibleTime = 5.0f;

	bool IsDeadAlready = false;
};
