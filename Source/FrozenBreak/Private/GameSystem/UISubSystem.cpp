// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/UISubSystem.h"
#include "GameSystem/GameManager.h"
#include "Player/ThirdPersonPlayerController.h"
#include "Blueprint/UserWidget.h"

UUISubSystem* UUISubSystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (const UWorld* World = WorldContextObject->GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				// GameInstance에서 서브시스템을 가져옵니다.
				return GameInstance->GetSubsystem<UUISubSystem>();
			}
		}
	}
	return nullptr;
}

UUserWidget* UUISubSystem::ShowWidget(EWidgetType InWidgetType)
{
	if (CurrentPlayerController == nullptr)
	{
		CurrentPlayerController = GetWorld()->GetFirstPlayerController();
	}

	//GameMode로 막을지 여부...

	UUserWidget* Widget = nullptr;

	if (CreatedWidgets.Contains(InWidgetType))
	{
		Widget = CreatedWidgets[InWidgetType];

		if (Widget->IsInViewport() == false)
		{
			Widget->AddToViewport();
		}
		Widget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		if (UGameManager* GameMgr = UGameManager::Get(this))
		{
			TSubclassOf<UUserWidget> WidgetClass = GameMgr->GetWidgetClass(InWidgetType);

			if (WidgetClass)
			{
				Widget = CreateWidget(CurrentPlayerController, WidgetClass);
			}
		}

		if (Widget)
		{
			CreatedWidgets.Add(InWidgetType, Widget);
			Widget->AddToViewport();
			return Widget;
		}
	}

	return Widget;
}

void UUISubSystem::HideWidget(EWidgetType InWidgetType)
{
	if (CreatedWidgets.Contains(InWidgetType))
	{
		CreatedWidgets[InWidgetType]->SetVisibility(ESlateVisibility::Collapsed);
	}
}
