// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/PlayerStatComponent.h"
#include <GameSystem/EventSubSystem.h>

// Sets default values for this component's properties
UPlayerStatComponent::UPlayerStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
void UPlayerStatComponent::SetPlayerTemperature(float InTemperatureValue)
{
	if (CurrentTemperature > 0 || InTemperatureValue > 0)
	{
		CurrentTemperature = FMath::Clamp(CurrentTemperature + InTemperatureValue, 0, MaxTemperature);
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Status.OnTemperaturePointChanged.Broadcast(CurrentTemperature / MaxTemperature);
		}
	}
	else
	{
		// 온도 값이 0이 되어 체력이 소모되기 시작
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			//TODO : Damage 호출로 수정필요
			EventSystem->Status.OnTemperaturePointChanged.Broadcast(CurrentTemperature / MaxTemperature);
		}
	}
}

void UPlayerStatComponent::SetPlayerFatigue(float InFatigueValue)
{
	if (CurrentFatigue > 0 || InFatigueValue > 0)
	{
		CurrentFatigue = FMath::Clamp(CurrentFatigue + InFatigueValue, 0, MaxFatigue);
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Status.OnFatiguePointChanged.Broadcast(CurrentFatigue / MaxFatigue);
		}
	}
	else
	{
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			//TODO : Damage 호출로 수정필요
			EventSystem->Status.OnFatiguePointChanged.Broadcast(CurrentTemperature / MaxTemperature);
		}
	}

}

void UPlayerStatComponent::SetPlayerHunger(float InHungerValue)
{
	if (CurrentHunger > 0 || InHungerValue > 0)
	{
		CurrentHunger = FMath::Clamp(CurrentHunger + InHungerValue, 0, MaxHunger);
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Status.OnHungerPointChanged.Broadcast(CurrentHunger / MaxHunger);
		}
	}
	else
	{

		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			//TODO : Damage 호출로 수정필요
			EventSystem->Status.OnHungerPointChanged.Broadcast(CurrentHunger / MaxHunger);
		}
	}

}