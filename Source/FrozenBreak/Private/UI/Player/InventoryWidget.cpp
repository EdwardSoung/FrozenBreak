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
		EventSystem->Character.OnUpdateItem.AddDynamic(this, &UInventoryWidget::UpdateItem);		

		EventSystem->UI.OnInventoryItemSelected.AddDynamic(this, &UInventoryWidget::SelectionChanged);

		//최초에 생성되면..초기화 요청
		EventSystem->Character.OnRequestInventoryInit.Broadcast();
	}

	TrashButton->OnClicked.AddDynamic(this, &UInventoryWidget::DropItem);
	CloseButton->OnClicked.AddDynamic(this, &UInventoryWidget::HideWidget);
	UseButton->OnClicked.AddDynamic(this, &UInventoryWidget::UseItem);
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	int32 numberKey = 0;
	
	if (InKeyEvent.GetKey() == EKeys::One)
		numberKey = 1;
	else if (InKeyEvent.GetKey() == EKeys::Two)
		numberKey = 2;
	else if (InKeyEvent.GetKey() == EKeys::Three)
		numberKey = 3;
	else if (InKeyEvent.GetKey() == EKeys::Four)
		numberKey = 4;
	else if (InKeyEvent.GetKey() == EKeys::Five)
		numberKey = 5;

	if (numberKey > 0)
	{
		auto Selected = InventoryList->GetSelectedItem();

		if (Selected)
		{
			//이거 하기전에 현재 인벤토리에서 이 번호 아이템 있으면 슬롯 지워야함


			//퀵슬롯 등록
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->UI.OnResetQuickSlotItem.Broadcast(numberKey);

				UInventoryItem* selectedItem = Cast<UInventoryItem>(Selected);
				selectedItem->QuickSlotNum = numberKey;
				EventSystem->UI.OnSetItemToQuickSlot.Broadcast(numberKey, selectedItem);
			}
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
}

void UInventoryWidget::UpdateItem(UInventoryItem* InItem)
{
	if (InItem->GetAmount() == 0)
	{
		InventoryList->RemoveItem(InItem);
		ItemDataList.Remove(InItem);
	}
	else
	{
		for (auto data : ItemDataList)
		{
			if (data->GetUID() == InItem->GetUID())
			{
				data = InItem;
			}
		}
	}	
}

//아이템 밖에 버림 처리
void UInventoryWidget::DropItem()
{
	//어...참조라 지우면 같이 지워지지 않나?
	auto Selected = InventoryList->GetSelectedItem();

	if (Selected)
	{
		UInventoryItem* selectedItem = Cast<UInventoryItem>(Selected);
		int32 quickSlot = selectedItem->QuickSlotNum;
		InventoryList->RemoveItem(Selected);
		ItemDataList.Remove(selectedItem);

		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnDropItem.Broadcast(selectedItem);

			if (quickSlot > 0)
			{
				EventSystem->UI.OnResetQuickSlotItem.Broadcast(quickSlot);
			}
		}
	}
}

//사용/장착 버튼을 눌렀음
void UInventoryWidget::UseItem()
{
	auto Selected = InventoryList->GetSelectedItem();

	if (Selected)
	{
		UInventoryItem* selectedItem = Cast<UInventoryItem>(Selected);

		//그냥 InventoryComponent로 사용을 알림	
		if (selectedItem)
		{
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnUseItem.Broadcast(selectedItem);
				selectedItem->QuickSlotNum = 0;
			}
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
	//캠프파이어 아이템은 떨구는 아이템이 없는 상태라 버리기 비활성
	TrashButton->SetVisibility(InType == EItemType::Campfire ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

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
		break;
	}
}

void UInventoryWidget::ResetQuickSlotNumber(uint32 UID)
{
	//퀵슬롯에 이미 아이템이 있는 경우 기존 아이템 퀵슬롯 제거
	for (auto Item : ItemDataList)
	{
		if (Item->GetUID() == UID)
		{
			Item->QuickSlotNum = 0;
			break;
		}
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
