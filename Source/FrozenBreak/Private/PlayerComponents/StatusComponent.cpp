// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/StatusComponent.h"
#include "GameSystem/EventSubSystem.h"

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatusComponent::SetPlayerTemperature(float InTemperatureValue)
{
	if (CurrentTemperature > 0)
	{
		CurrentTemperature = FMath::Clamp(CurrentTemperature + InTemperatureValue, DefaultMinimum, MaxTemperature);
	}
	else
	{
		// 온도 값이 0이 되어 체력이 소모되기 시작
		SetPlayerHealth(InTemperatureValue);
	}
}

void UStatusComponent::SetPlayerHealth(float InHealthValue)
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
void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	InitStatus();
}

void UStatusComponent::InitStatus()
{
	// 체력 초기 값 세팅
	CurrentHealth = MaxHealth;
	SetPlayerHealth(CurrentHealth);

	// 온도 초기 값 세팅
	CurrentTemperature = MaxTemperature;
}