// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/PlayerStatusWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GameSystem/EventSubSystem.h"
#include "GameSystem/UISubSystem.h"
#include "UI/Player/EquipmentSlotWidget.h"
#include "Data/ItemData.h"
#include "Objects/InventoryItem.h"

void UPlayerStatusWidget::NativeConstruct()
{
	SetInGameTime(DefaultGameTime);
	SetTemperatureDefence(DefaultTemperatureDefence);

	if (UEventSubSystem* statusEvent = UEventSubSystem::Get(this))
	{
		statusEvent->Status.OnInGameTimeChanged.AddDynamic(this, &UPlayerStatusWidget::SetInGameTime);
		statusEvent->Status.OnTemperatureDefenceChanged.AddDynamic(this, &UPlayerStatusWidget::SetTemperatureDefence);
		statusEvent->Character.OnEquipHandItem.AddDynamic(this, &UPlayerStatusWidget::SetHandEquipment);
		statusEvent->Character.OnEquipBodyItem.AddDynamic(this, &UPlayerStatusWidget::SetBodyEquipment);
		statusEvent->Status.OnDayChanged.AddDynamic(this, &UPlayerStatusWidget::SetDay);
		statusEvent->Character.OnEquipRefresh.AddDynamic(this, &UPlayerStatusWidget::RefreshEquipment);

		statusEvent->Character.OnRequestStatusInit.Broadcast();
	}
	CloseStatButton->OnClicked.AddDynamic(this, &UPlayerStatusWidget::HideWidget);
}

void UPlayerStatusWidget::SetInGameTime(FText InTimeValue)
{
	if (InGameTime)	
	{
		InGameTime->SetText(InTimeValue);
	}
}

void UPlayerStatusWidget::SetTemperatureDefence(FText InDefenceValue)
{
	if (TemperatureDefence)
	{
		TemperatureDefence->SetText(InDefenceValue);
	}
}

void UPlayerStatusWidget::SetHandEquipment(UInventoryItem* InHandItem)
{
	if (InHandItem)
	{
		switch (InHandItem->GetType())
		{
		case EItemType::Axe:
		case EItemType::Pickaxe:
		case EItemType::Knife:
			break;
		default:
			return;
		}
		if (HandEquipment)
		{
			HandEquipment->RefreshSlot(InHandItem);
		}
	}
	else
	{
		if (HandEquipment)
		{
			HandEquipment->RefreshSlot(InHandItem);
		}
	}
}

void UPlayerStatusWidget::SetBodyEquipment(UInventoryItem* InBodyItem)
{
	switch (InBodyItem->GetType())
	{
	case EItemType::Jaket:
		break;
	default:
		return;
	}
	if (BodyEquipment)
	{
		BodyEquipment->RefreshSlot(InBodyItem);
	}
}

void UPlayerStatusWidget::HideWidget()
{
	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->HideWidget(EWidgetType::CharacterState);
	}
}

void UPlayerStatusWidget::SetDay(int32 InDay)
{
	DayText->SetText(FText::AsNumber(InDay));
}

void UPlayerStatusWidget::RefreshEquipment(UInventoryItem* InHandItem, UInventoryItem* InBodyItem)
{
	if (InHandItem)
		SetHandEquipment(InHandItem);

	if (InBodyItem)
		SetBodyEquipment(InBodyItem);
}


void UPlayerStatusWidget::OpenWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);

}

void UPlayerStatusWidget::CloseWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Collapsed);
}