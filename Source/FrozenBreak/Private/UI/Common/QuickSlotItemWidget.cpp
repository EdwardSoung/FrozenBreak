// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/QuickSlotItemWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UQuickSlotItemWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//시작하면 꺼줌
	if (ItemIcon)
	{
		ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UQuickSlotItemWidget::SetSlotNumber(int32 InNumber)
{
	if (SlotNumberText)
	{
		SlotNumberText->SetText(FText::AsNumber(InNumber));
	}
}

void UQuickSlotItemWidget::SetIcon(UTexture2D* InTexture)
{
	if (ItemIcon)
	{
		if (InTexture)
		{
			ItemIcon->SetBrushFromTexture(InTexture);
			ItemIcon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
