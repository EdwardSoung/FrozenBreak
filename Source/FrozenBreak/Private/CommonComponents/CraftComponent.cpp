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
		EventSystem->Chraracter.OnStartCraft.AddDynamic(this, &UCraftComponent::StartCraft);
	}
}

// Current Craft Cost 값(진행 작업량) 셋팅과 UI 갱신
void UCraftComponent::SstCurrentCraftCost(float InCraftAmount)
{
	if (InCraftAmount > 0 && MaxCraftCost > 0)
	{
		if (CurrentCraftCost < MaxCraftCost)
		{
			CurrentCraftCost = FMath::Clamp(CurrentCraftCost + InCraftAmount, 0, MaxCraftCost);
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Status.OnCurrentCraftCostChanged.Broadcast(CurrentCraftCost);
			}
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(CraftHandle);
			// TODO : Craft 완료 처리(이벤트 날리기 등)
		}
	}
	else
	{
		// TODO : Craft Amount(작업력) MaxCraftCost(작업량) 0일 경우 크래프트 실패 처리. 그냥 무시도 가능
	}
}

void UCraftComponent::TestCraft(float InCraftAmount)
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Chraracter.OnStartCraft.Broadcast(InCraftAmount);
	}
}

// Craft 시작
void UCraftComponent::StartCraft(float InCraftAmount)
{
	GetWorld()->GetTimerManager().ClearTimer(CraftHandle);

	CraftAmount = InCraftAmount;
	GetWorld()->GetTimerManager().SetTimer(
		CraftHandle,
		[this]()
		{
			SstCurrentCraftCost(CraftAmount);
		},
		CraftRate,
		true
	);
}
