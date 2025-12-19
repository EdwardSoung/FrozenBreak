// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/CraftInventroyWidget.h"
#include "Objects/InventoryItem.h"
#include "Components/TileView.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include <UI/Player/CraftableItemSlot.h>
#include <GameSystem/EventSubSystem.h>
#include <GameSystem/UISubSystem.h>

void UCraftInventroyWidget::NativeConstruct()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Chraracter.OnInitCraftUI.AddDynamic(this, &UCraftInventroyWidget::InitCraftableData);
		EventSystem->Chraracter.OnAddItemToCraftInventoryUI.AddDynamic(this, &UCraftInventroyWidget::AddCraftItem);
		EventSystem->Chraracter.OnUpdateCraftItem.AddDynamic(this, &UCraftInventroyWidget::UpdateCraftItemByType);
		EventSystem->Chraracter.OnRemoveCraftItem.AddDynamic(this, &UCraftInventroyWidget::RemoveCraftItem);

		EventSystem->Chraracter.OnRequestIventoryItems.Broadcast();

	}
	OnNativeVisibilityChanged.AddUFunction(this, "RequestInventoryData");

	CraftButton->OnClicked.AddDynamic(this, &UCraftInventroyWidget::StartCraft);
	CloseCraftButton->OnClicked.AddDynamic(this, &UCraftInventroyWidget::CloseCraft);
}

void UCraftInventroyWidget::StartCraft()
{
	auto Selected = CarftInventoryList->GetSelectedItem();

	if (Selected)
	{
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			UInventoryItem* ItemToCraft = Cast<UInventoryItem>(Selected);
			EventSystem->Chraracter.OnCraftRequested.Broadcast(ItemToCraft);
			CloseCraft();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Nothing Selected"));
	}
}

void UCraftInventroyWidget::UpdateCraftItemByType(EItemType InType)
{
	for (auto Item : CraftItemDataList)
	{
		if (Item->GetData()->ItemType == InType)
		{
			auto itemWidget = CarftInventoryList->GetEntryWidgetFromItem(Item);
			if (UCraftableItemSlot* slot = Cast<UCraftableItemSlot>(itemWidget))
			{
				slot->NativeOnListItemObjectSet(Item);
			}
			break;
		}
	}
}

void UCraftInventroyWidget::RequestInventoryData()
{
	if (this->GetVisibility() == ESlateVisibility::Visible)
	{
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Chraracter.OnRequestIventoryItems.Broadcast();
		}
		CarftInventoryList->ClearSelection();
	}
}

void UCraftInventroyWidget::AddCraftItem(UInventoryItem* InItem)
{
	CraftItemDataList.Add(InItem);
	CarftInventoryList->AddItem(InItem);
}

void UCraftInventroyWidget::InitCraftableData(TArray<class UInventoryItem*> InData)
{
	CraftItemDataList = InData;

	if (CarftInventoryList)
	{
		CarftInventoryList->ClearListItems();
	}

	for (UInventoryItem* Data : CraftItemDataList)
	{
		if (!Data) continue;
		CarftInventoryList->AddItem(Data);
	}

	CarftInventoryList->RequestRefresh();
}

void UCraftInventroyWidget::RemoveCraftItem(UInventoryItem* InItem)
{
	if (!InItem || !CarftInventoryList) return;

	// 1) 내 캐시 배열에서 제거
	CraftItemDataList.RemoveSingle(InItem);

	// 2) TileView에서 제거
	CarftInventoryList->RemoveItem(InItem);

	// 3) 화면 즉시 갱신
	CarftInventoryList->RequestRefresh();
}

void UCraftInventroyWidget::CloseCraft()
{
	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->HideWidget(EWidgetType::CraftInventory);
	}
}
