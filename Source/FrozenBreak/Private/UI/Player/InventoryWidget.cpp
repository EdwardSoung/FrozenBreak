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
		EventSystem->Chraracter.OnInitInventoryUI.AddDynamic(this, &UInventoryWidget::InitData);
		EventSystem->Chraracter.OnAddItemToInventoryUI.AddDynamic(this, &UInventoryWidget::AddItem);
		EventSystem->Chraracter.OnUpdateInventoryItem.AddDynamic(this, &UInventoryWidget::UpdateItemByType);
		EventSystem->Chraracter.OnUpdateInventoryWeight.AddDynamic(this, &UInventoryWidget::UpdateWeight);

		//최초에 생성되면..초기화 요청
		EventSystem->Chraracter.OnRequestInventoryInit.Broadcast();
	}

	TrashButton->OnClicked.AddDynamic(this, &UInventoryWidget::TrashItem);
	CloseButton->OnClicked.AddDynamic(this, &UInventoryWidget::HideWidget);
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::I)
	{		
		//음..굳이..? 포커스도 맞추기 힘든..
		//HideWidget();
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
			EventSystem->Chraracter.OnTrashItem.Broadcast(selectedItem);
		}
	}
}

void UInventoryWidget::UseItem()
{
	auto Selected = InventoryList->GetSelectedItem();

	if (Selected)
	{
		UInventoryItem* selectedItem = Cast<UInventoryItem>(Selected);
		
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

void UInventoryWidget::OpenWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);

	InventoryList->ClearSelection();
}

void UInventoryWidget::CloseWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
