// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonComponents/CraftComponent.h"
#include "GameSystem/EventSubSystem.h"

void UCraftComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentCraftCost = BaseCraftCost;
	BindStatSettingEvents();
}

void UCraftComponent::BindStatSettingEvents()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Chraracter.OnSetCraftCost.AddDynamic(this, &UCraftComponent::SstCraftCost);
	}
}

void UCraftComponent::CraftItem()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Chraracter.OnSetCraftCost.Broadcast(CurrentCraftAmount);
	}
}

void UCraftComponent::SstCraftCost(float InCraftAmount)
{
	if (InCraftAmount > 0 && CurrentCraftCost < MaxCraftCost)
	{
		CurrentCraftCost = FMath::Clamp(CurrentCraftCost + InCraftAmount, 0, MaxCraftCost);
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Status.OnCurrentCraftCostChanged.Broadcast(CurrentCraftCost);
		}
	}
}
