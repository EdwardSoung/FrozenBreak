// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonComponents/StatComponent.h"
#include "Interface/Damageable.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UStatComponent::InitStat(float InHealth, float InAttack)
{
	Attack = InAttack;
	MaxHealth = InHealth;
	CurrentHealth = InHealth;
}

void UStatComponent::OnDamaged(float InDamage)
{
	if (MaxHealth < 0)
	{
		//체력 -1이면 무적
		return;
	}

	if (CurrentHealth > 0)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - InDamage, 0, MaxHealth);

	}
	else
	{
		//뭔지몰라도 사망
		if (GetOwner()->Implements<UDamageable>())
		{
			IDamageable::Execute_OnDead(GetOwner());
		}
	}
}