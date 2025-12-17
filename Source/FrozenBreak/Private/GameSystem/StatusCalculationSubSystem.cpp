// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/StatusCalculationSubSystem.h"
#include "GameSystem/EventSubSystem.h"

void UStatusCalculationSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EventSystem = UEventSubSystem::Get(this);
}

void UStatusCalculationSubSystem::Deinitialize()
{
	Super::Deinitialize();
}

void UStatusCalculationSubSystem::SetTemperature()
{
	if (EventSystem) EventSystem->Status.OnSetTemperature.Broadcast(StatusDataAsset->Temperature);
}

void UStatusCalculationSubSystem::SetFatigue()
{
	if (EventSystem) EventSystem->Status.OnSetFatigue.Broadcast(StatusDataAsset->Fatigue);
}

void UStatusCalculationSubSystem::SetHunger()
{
	if (EventSystem) EventSystem->Status.OnSetHunger.Broadcast(StatusDataAsset->Hunger);
}

void UStatusCalculationSubSystem::StartStatLoop()
{
	StartTemperatureLoop();
	StartFatigueLoop();
	StartHungerLoop();
}

void UStatusCalculationSubSystem::StartTemperatureLoop()
{
	UWorld* World = GetWorld();
	if (World && StatusDataAsset)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TemperatureTimer,
			this,
			&UStatusCalculationSubSystem::SetTemperature,
			StatusDataAsset->TemperatureTimerRate,
			true
		);
	}
}

void UStatusCalculationSubSystem::StartFatigueLoop()
{
	UWorld* World = GetWorld();
	if (World && StatusDataAsset)
	{
		GetWorld()->GetTimerManager().SetTimer(
			FatigueTimer,
			this,
			&UStatusCalculationSubSystem::SetFatigue,
			StatusDataAsset->FatigueTimerRate,
			true
		);
	}
}

void UStatusCalculationSubSystem::StartHungerLoop()
{
	UWorld* World = GetWorld();
	if (World && StatusDataAsset)
	{
		GetWorld()->GetTimerManager().SetTimer(
			HungerTimer,
			this,
			&UStatusCalculationSubSystem::SetHunger,
			StatusDataAsset->HungerTimerRate,
			true
		);
	}
}