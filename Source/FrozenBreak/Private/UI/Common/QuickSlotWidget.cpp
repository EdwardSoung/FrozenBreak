// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/QuickSlotWidget.h"
#include "UI/Common/QuickSlotItemWidget.h"
#include "GameSystem/EventSubSystem.h"
#include "Components/HorizontalBox.h"

void UQuickSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

    if (SlotParent)
    {
        // 컨테이너의 모든 자식들을 순회
        TArray<UWidget*> Children = SlotParent->GetAllChildren();

        for (UWidget* Child : Children)
        {
            if (UQuickSlotItemWidget* Item = Cast<UQuickSlotItemWidget>(Child))
            {
                Slots.Add(Item);
            }
        }

    }
    for (int32 i = 0; i < Slots.Num(); i++)
    {
        Slots[i]->SetSlotNumber(i + 1);
    }

	if (UEventSubSystem* Event = UEventSubSystem::Get(this))
	{
        Event->UI.OnSetItemToQuickSlot.AddDynamic(this, &UQuickSlotWidget::UpdateSlot);
	}
}

void UQuickSlotWidget::UpdateSlot(int32 InIndex, UInventoryItem* InItem)
{
    //Slots[InIndex - 1]->updateslot
}
