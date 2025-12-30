// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/PlayerStatComponent.h"
#include "Data/StatusDataAsset.h"
#include <GameSystem/EventSubSystem.h>
#include <GameSystem/StatusCalculationSubSystem.h>
#include "Objects/InventoryItem.h"

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

		EventSystem->Character.OnUseItem.AddDynamic(this, &UPlayerStatComponent::ItemUsed);
		EventSystem->Character.OnEquipHandItemUsed.AddDynamic(this, &UPlayerStatComponent::EquipHandItemUsed);

		EventSystem->Character.OnRequestStatusInit.AddDynamic(this, &UPlayerStatComponent::RefreshEquipments);
	}
}

//아이템이 사용(장착)됨
void UPlayerStatComponent::ItemUsed(UInventoryItem* InItem)
{
	switch (InItem->GetType())
	{
	case EItemType::Knife:
	case EItemType::Axe:
	case EItemType::Pickaxe:
		if (HandEquip)
		{
			//현재 아이템 있으면 현재 아이템은 인벤토리로
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				//아이템 사용 차감을 동일하게 가져가고, 여기를 우선 다시 세팅..
				HandEquip->SetAmount(1);
				EventSystem->Character.OnAddItemToInventoryUI.Broadcast(HandEquip);
			}
		}
		HandEquip = InItem;
		break;

	case EItemType::Jaket:

		if (BodyEquip)
		{
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				BodyEquip->SetAmount(1);
				EventSystem->Character.OnAddItemToInventoryUI.Broadcast(BodyEquip);
			}
		}
		BodyEquip = InItem;

		if (auto coldStat = BodyEquip->GetData()->Stats.Find(EItemStatType::ColdResistance))
		{
			UStatusCalculationSubSystem* StatusCalculater = GetWorld()->GetSubsystem<UStatusCalculationSubSystem>();
			StatusCalculater->SetTemperatureDefence(*coldStat);
		}

		break;
	case EItemType::CookedMeat:
	case EItemType::Fruit:

		if (float* fatigueValue = InItem->GetData()->Stats.Find(EItemStatType::Fatigue))
		{
			float value = *fatigueValue;
			SetPlayerFatigue(value);
		}
		if (float* hungerValue = InItem->GetData()->Stats.Find(EItemStatType::Hunger))
		{
			float value = *hungerValue;
			SetPlayerHunger(value);
		}
		if (float* temperatureValue = InItem->GetData()->Stats.Find(EItemStatType::Temperature))
		{
			float value = *temperatureValue;
			SetPlayerTemperature(value);
		}
		break;

	default:
		break;
	}
}

void UPlayerStatComponent::EquipHandItemUsed()
{
	if (HandEquip)
	{
		HandEquip->ItemUsed();

		if (HandEquip->GetDurability() <= 0)
		{
			HandEquip = nullptr;
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnEquipHandItem.Broadcast(nullptr);
			}
		}
	}
	else
	{
		//캐릭터에 무기 없애기
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnEquipHandItem.Broadcast(nullptr);
		}
	}
}

void UPlayerStatComponent::RefreshEquipments()
{
	if (HandEquip || BodyEquip)
	{
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnEquipRefresh.Broadcast(HandEquip, BodyEquip);
		}
	}
}

void UPlayerStatComponent::SendCurrentFatigue()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Character.OnFatigueChecked.Broadcast(CurrentFatigue);
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
	}/*
	else
	{
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Status.OnSetHealth.Broadcast(InFatigueValue);
		}
	}*/

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
