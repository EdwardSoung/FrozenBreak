// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/QuickSlotItemWidget.h"
#include "Objects/InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameSystem/EventSubSystem.h"

void UQuickSlotItemWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//시작하면 꺼줌
	if (ItemIcon)
	{
		ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ItemAmountText)
		ItemAmountText->SetVisibility(ESlateVisibility::Collapsed);
	
	if (UEventSubSystem* Event = UEventSubSystem::Get(this))
	{
		Event->Character.OnUseItem.AddDynamic(this, &UQuickSlotItemWidget::ItemUsed);
		Event->UI.OnSetItemToQuickSlot.AddDynamic(this, &UQuickSlotItemWidget::SetItem);
	}
}

void UQuickSlotItemWidget::ItemUsed(UInventoryItem* InItem)
{
	if (InItem->QuickSlotNum > 0 && InItem->QuickSlotNum == SlotNumber)
	{
		switch (InItem->GetType())
		{
		case EItemType::Axe:
		case EItemType::Pickaxe:
		case EItemType::Knife:
		case EItemType::Campfire:
		case EItemType::Jaket:
			SetSlotData(nullptr);
			break;
		case EItemType::CookedMeat:
		case EItemType::Fruit:
			SetSlotData(InItem);
			break;
		default:
			//사용불가능 아이템은 패스
			break;
		}
	}
}

void UQuickSlotItemWidget::SetItem(int32 InSlotNum, UInventoryItem* InItem)
{
	if (InSlotNum == SlotNumber)
	{
		SetSlotData(InItem);
	}
}

void UQuickSlotItemWidget::SetSlotNumber(int32 InNumber)
{
	SlotNumber = InNumber;
	if (SlotNumberText)
	{
		SlotNumberText->SetText(FText::AsNumber(InNumber));
	}
}

void UQuickSlotItemWidget::SetSlotData(UInventoryItem* InItem)
{
	if (InItem)
	{
		if (ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(InItem->GetData()->ItemIcon);
			ItemIcon->SetVisibility(ESlateVisibility::Visible);
		}

		if (ItemAmountText)
		{
			ItemAmountText->SetText(FText::AsNumber(InItem->GetAmount()));
			ItemAmountText->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (ItemAmountText)
			ItemAmountText->SetVisibility(ESlateVisibility::Collapsed);

		if (ItemIcon)
			ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}
