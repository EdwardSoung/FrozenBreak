// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROZENBREAK_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void InitStat(float InHealth, float InAttack = 0);
	void OnDamaged(float InDamage);

	// ==== 프롭 상호작용 테스트용 입니다 ====
	void OnPropDamaged(float InDamage);
public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	float Attack = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentHealth = -1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxHealth = -1;
};
