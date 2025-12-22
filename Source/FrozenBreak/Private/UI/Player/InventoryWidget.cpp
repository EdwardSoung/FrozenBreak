// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/InventoryWidget.h"
#include "UI/Player/InventoryItemSlot.h"
#include "Objects/InventoryItem.h"
#include "GameSystem/EventSubSystem.h"
#include "GameSystem/UISubSystem.h"
#include "Components/TileView.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();	

	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Character.OnInitInventoryUI.AddDynamic(this, &UInventoryWidget::InitData);
		EventSystem->Character.OnAddItemToInventoryUI.AddDynamic(this, &UInventoryWidget::AddItem);
		EventSystem->Character.OnUpdateInventoryItem.AddDynamic(this, &UInventoryWidget::UpdateItemByType);
		EventSystem->Character.OnUpdateInventoryWeight.AddDynamic(this, &UInventoryWidget::UpdateWeight);

		EventSystem->UI.OnInventoryItemSelected.AddDynamic(this, &UInventoryWidget::SelectionChanged);

		//최초에 생성되면..초기화 요청
		EventSystem->Character.OnRequestInventoryInit.Broadcast();
	}

	TrashButton->OnClicked.AddDynamic(this, &UInventoryWidget::TrashItem);
	CloseButton->OnClicked.AddDynamic(this, &UInventoryWidget::HideWidget);
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::One)
	{		
		auto Selected = InventoryList->GetSelectedItem();

		if (Selected)
		{

		}
		return FReply::Handled();
	}

	// 다른 키는 부모 클래스 루틴 따름 (Unhandled 반환)
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UInventoryWidget::UpdateItemByType(EItemType InType)
{
	for (auto Item : ItemDataList)
	{
		if (Item->GetData()->ItemType == InType)
		{
			auto itemWidget = InventoryList->GetEntryWidgetFromItem(Item);
			if (UInventoryItemSlot* slot = Cast<UInventoryItemSlot>(itemWidget))
			{
				slot->NativeOnListItemObjectSet(Item);
			}
			break;
		}
	}
}

void UInventoryWidget::AddItem(UInventoryItem* InItem)
{
	ItemDataList.Add(InItem);
	InventoryList->AddItem(InItem);
}

void UInventoryWidget::InitData(TArray<class UInventoryItem*> InData)
{
	ItemDataList = InData;
	for (auto data : ItemDataList)
	{
		InventoryList->AddItem(data);
	}

	//FString weightValue = FString::Printf(TEXT("%.2f"), Weight);
	//WeightText->SetText(FText::FromString(weightValue));
}

void UInventoryWidget::RemoveItem(UInventoryItem* InItem)
{
}

void UInventoryWidget::TrashItem()
{
	//어...참조라 지우면 같이 지워지지 않나?
	auto Selected = InventoryList->GetSelectedItem();

	if (Selected)
	{
		//InventoryList->ClearSelection();
		InventoryList->RemoveItem(Selected);
		UInventoryItem* selectedItem = Cast<UInventoryItem>(Selected);
		ItemDataList.Remove(selectedItem);

		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnTrashItem.Broadcast(selectedItem);
		}
	}
}

void UInventoryWidget::UseItem()
{
	auto Selected = InventoryList->GetSelectedItem();

	if (Selected)
	{
		UInventoryItem* selectedItem = Cast<UInventoryItem>(Selected);

		switch (selectedItem->GetData()->ItemType)
		{
		case EItemType::Axe:
		case EItemType::Pickaxe:
		case EItemType::Knife:
		case EItemType::Jaket:
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnEquipInventoryItem.Broadcast(selectedItem);
			}
			break;
		case EItemType::CookedMeat:
		case EItemType::Fruit:
		case EItemType::Campfire:
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnUseInventoryItem.Broadcast(selectedItem);
			}
			break;
		default:
			break;
		}
	}
}

void UInventoryWidget::HideWidget()
{	
	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->HideWidget(EWidgetType::Inventory);
	}
}

void UInventoryWidget::UpdateWeight(float InWeight, float InMaxWeight)
{
	FString weightValue = FString::Printf(TEXT("%.2f/%.2f"), InWeight, InMaxWeight);
	WeightText->SetText(FText::FromString(weightValue));
}

void UInventoryWidget::SelectionChanged(EItemType InType)
{
	switch (InType)
	{
	case EItemType::Axe:
	case EItemType::Pickaxe:
	case EItemType::Knife:
	case EItemType::Jaket:
		//버튼 텍스트 착용으로 변경
		UseButton->SetIsEnabled(true);
		UseButtonText->SetText(FText::FromString(TEXT("착용")));
		break;
	case EItemType::CookedMeat:
	case EItemType::Fruit:
	case EItemType::Campfire:
		//버튼 텍스트 사용으로 변경
		UseButton->SetIsEnabled(true);
		UseButtonText->SetText(FText::FromString(TEXT("사용")));
		break;
	default:
		UseButtonText->SetText(FText::FromString(TEXT("사용")));
		UseButton->SetIsEnabled(false);
		//나머지 음..버튼 비활성?
	}
}

void UInventoryWidget::OpenWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);

	InventoryList->ClearSelection();
}

void UInventoryWidget::CloseWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
