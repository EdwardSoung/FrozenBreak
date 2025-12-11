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
	if (CurrentTemperature > 0)
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
}