// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/StatusComponent.h"
#include <GameSystem/EventSubSystem.h>

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatusComponent::SetPlayerTemperature(float InTemperatureValue)
{
	if (CurrentTemperature > 0 || InTemperatureValue > 0)
	{
		CurrentTemperature = FMath::Clamp(CurrentTemperature + InTemperatureValue, DefaultMinimum, MaxTemperature);
		if (UEventSubSystem* EventSystem = UEventSubSystem::GetEventSystem(this))
		{
			EventSystem->Status.OnTemperaturePointChanged.Broadcast(CurrentTemperature);
		}
	}
	else
	{
		// 온도 값이 0이 되어 체력이 소모되기 시작
		SetPlayerHealth(InTemperatureValue);
	}

	//UE_LOG(LogTemp, Log, TEXT("Current Temperature : %.2f / %.2f"), CurrentTemperature, MaxTemperature);
	//UE_LOG(LogTemp, Log, TEXT("Current Health : %.2f / %.2f"), CurrentHealth, MaxHealth);
}

void UStatusComponent::SetPlayerFatigue(float InFatigueValue)
{
	if (CurrentFatigue < MaxFatigue || InFatigueValue <= 0)
	{
		CurrentFatigue = FMath::Clamp(CurrentFatigue + InFatigueValue, DefaultMinimum, MaxFatigue);
	}
	else
	{
		SetPlayerHealth(-InFatigueValue);
	}

	//UE_LOG(LogTemp, Log, TEXT("Current Fatigue : %.2f / %.2f"), CurrentFatigue, MaxFatigue);
	//UE_LOG(LogTemp, Log, TEXT("Current Health : %.2f / %.2f"), CurrentHealth, MaxHealth);
}

void UStatusComponent::SetPlayerHunger(float InHungerValue)
{
	if (CurrentHunger > 0 || InHungerValue > 0)
	{
		CurrentHunger = FMath::Clamp(CurrentHunger + InHungerValue, DefaultMinimum, MaxHunger);
	}
	else
	{
		SetPlayerHealth(InHungerValue);
	}

	//UE_LOG(LogTemp, Log, TEXT("Current Hunger : %.2f / %.2f"), CurrentHunger, MaxHunger);
	//UE_LOG(LogTemp, Log, TEXT("Current Health : %.2f / %.2f"), CurrentHealth, MaxHealth);
}

// Called when the game starts
void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatusComponent::InitStatus()
{
	Super::InitStatus();

	// 온도 초기 값 세팅
	CurrentTemperature = MaxTemperature;

	// 피로도 초기 값 세팅
	CurrentFatigue = DefaultMinimum;
	
	// 포만감 초기 값 세팅
	CurrentHunger = MaxHunger;
}