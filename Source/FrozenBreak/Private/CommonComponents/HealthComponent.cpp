// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonComponents/HealthComponent.h"
#include "GameSystem/EventSubSystem.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::SetPlayerHealth(float InHealthValue)
{
	if (CurrentHealth > 0)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth + InHealthValue, DefaultMinimum, MaxHealth);

		if (UEventSubSystem* EventSystem = UEventSubSystem::GetEventSystem(this))
		{
			EventSystem->Status.OnHealthPointChanged.Broadcast(CurrentHealth / MaxHealth);
		}
	}
	else
	{
		// TODO : 캐릭터 사망
	}
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	InitStatus();
}

void UHealthComponent::InitStatus()
{
	// 체력 초기 값 세팅
	CurrentHealth = MaxHealth;
	SetPlayerHealth(CurrentHealth);
}