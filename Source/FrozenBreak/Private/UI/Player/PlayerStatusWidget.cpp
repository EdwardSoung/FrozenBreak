// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/PlayerStatusWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GameSystem/EventSubSystem.h"
#include "GameSystem/UISubSystem.h"
#include "UI/Player/EquipmentSlotWidget.h"
#include "Data/ItemData.h"

void UPlayerStatusWidget::NativeConstruct()
{
	SetInGameTime(DefaultGameTime);
	SetTemperatureDefence(DefaultTemperatureDefence);

	if (UEventSubSystem* statusEvent = UEventSubSystem::Get(this))
	{
		statusEvent->Status.OnInGameTimeChanged.AddDynamic(this, &UPlayerStatusWidget::SetInGameTime);
		statusEvent->Status.OnTemperatureDefenceChanged.AddDynamic(this, &UPlayerStatusWidget::SetTemperatureDefence);
		statusEvent->Status.OnHandEquipmentChanged.AddDynamic(this, &UPlayerStatusWidget::SetHandEquipment);
		statusEvent->Status.OnBodyEquipmentChanged.AddDynamic(this, &UPlayerStatusWidget::SetBodyEquipment);
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

void UPlayerStatusWidget::SetHandEquipment(UItemData* InHandItem)
{
	if (HandEquipment)
	{
		HandEquipment->RefreshSlot(InHandItem);
	}
}

void UPlayerStatusWidget::SetBodyEquipment(UItemData* InBodyItem)
{
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


void UPlayerStatusWidget::OpenWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);

}

void UPlayerStatusWidget::CloseWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Collapsed);
}