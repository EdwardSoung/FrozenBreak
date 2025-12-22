// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/PlayerStatComponent.h"
#include "Data/StatusDataAsset.h"
#include <GameSystem/EventSubSystem.h>
#include <GameSystem/StatusCalculationSubSystem.h>

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
	InitStatus();
	BindStatSettingEvents();
}

void UPlayerStatComponent::InitStatus()
{
	// 온도
	CurrentTemperature = MaxTemperature;

	// 행동력
	CurrentFatigue = MaxFatigue;

	//현재 배고픔
	CurrentHunger = MaxHunger;

	if (StatusDataAsset)
	{
		UStatusCalculationSubSystem* StatusCalulater = GetWorld()->GetSubsystem<UStatusCalculationSubSystem>();
		StatusCalulater->SetStatusDataAsset(StatusDataAsset);
		StatusCalulater->StartStatLoop();
	}
}

void UPlayerStatComponent::BindStatSettingEvents()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Status.OnSetTemperature.AddDynamic(this, &UPlayerStatComponent::SetPlayerTemperature);
		EventSystem->Status.OnSetFatigue.AddDynamic(this, &UPlayerStatComponent::SetPlayerFatigue);
		EventSystem->Status.OnSetHunger.AddDynamic(this, &UPlayerStatComponent::SetPlayerHunger);

		//Add
		EventSystem->Character.OnEquipInventoryItem.AddDynamic(this, &UPlayerStatComponent::EquipItem);
		EventSystem->Character.OnUsableItemUsed.AddDynamic(this, &UPlayerStatComponent::EatItem);
	}
}

void UPlayerStatComponent::EquipItem(UInventoryItem* InItem)
{
	switch (InItem->GetType())
	{
	case EItemType::Axe:
	case EItemType::Pickaxe:
	case EItemType::Knife:
		if (HandEquip)
		{
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnAddItemToInventoryUI.Broadcast(HandEquip);
			}

			HandEquip = InItem;
			//업데이트
		}
		break;
	case EItemType::Jaket:
		if (BodyEquip)
		{
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnAddItemToInventoryUI.Broadcast(BodyEquip);
			}

			BodyEquip = InItem;
			//업데이트
		}
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("장착할 수 없는 아이템 장착 시도"));
		break;
	}
}

void UPlayerStatComponent::UseEquippedHandItem()
{
	//내구도 감소

	if (HandEquip)
	{
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnEquippedItemUsed.Broadcast();
		}
	}
}

void UPlayerStatComponent::EatItem(UItemData* InData)
{
	switch (InData->ItemType)
	{
	case EItemType::CookedMeat:
	case EItemType::Fruit:
		
		if (float* fatigueValue = InData->Stats.Find(EItemStatType::Fatigue))
		{
			float value = *fatigueValue;
			SetPlayerFatigue(value);
		}
		if (float* hungerValue = InData->Stats.Find(EItemStatType::Hunger))
		{
			float value = *hungerValue;
			SetPlayerHunger(value);
		}
		if (float* temperatureValue = InData->Stats.Find(EItemStatType::Temperature))
		{
			float value = *temperatureValue;
			SetPlayerTemperature(value);
		}
		break;
	default:
		//먹을 수 있는 아이템이 아님. 무시
		break;
	}
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
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Status.OnSetHealth.Broadcast(InTemperatureValue);
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
			EventSystem->Status.OnSetHealth.Broadcast(InFatigueValue);
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
			EventSystem->Status.OnSetHealth.Broadcast(InHungerValue);
		}
	}
}
