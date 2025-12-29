// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/UISubSystem.h"
#include "GameSystem/GameManager.h"
#include "Player/ThirdPersonPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/Player/InventoryWidget.h"

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

UUserWidget* UUISubSystem::ShowWidget(EWidgetType InWidgetType, EInputModeType InInputMode)
{
	//컨트롤러에 붙여주기 때문에 없으면 다시 연결 시도
	if (CurrentPlayerController.IsValid() == false)
	{
		CurrentPlayerController = GetWorld()->GetFirstPlayerController();
	}

	UUserWidget* Widget = nullptr;

	//생성된 적이 있으면 켜주고, 없으면 생성[Lazy Initialziation]
	if (CreatedWidgets.Contains(InWidgetType))
	{
		Widget = CreatedWidgets[InWidgetType];

		if (Widget->IsInViewport() == false)
		{
			Widget->AddToViewport();
		}
		if (Widget->Implements<UOpenable>())
		{
			IOpenable::Execute_OpenWidget(Widget);
		}
		else
		{
			Widget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (UGameManager* GameMgr = UGameManager::Get(this))
		{
			TSubclassOf<UUserWidget> WidgetClass = GameMgr->GetWidgetClass(InWidgetType);

			if (WidgetClass)
			{
				Widget = CreateWidget(CurrentPlayerController.Get(), WidgetClass);
			}
		}

		if (Widget)
		{
			CreatedWidgets.Add(InWidgetType, Widget);
			Widget->AddToViewport();
		}
	}

	if (Widget && InWidgetType != EWidgetType::HUD)
	{
		//기본 HUD가 아닐 때 처음 열어주는거면 인풋모드 변경
		if (OpenedWidgets.Num() == 0)
		{
			if (InInputMode == EInputModeType::GameAndUI)
			{
				FInputModeGameAndUI InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				CurrentPlayerController->SetInputMode(InputMode);
				CurrentPlayerController->bShowMouseCursor = true;
			}
			else if (InInputMode == EInputModeType::UIOnly)
			{
				FInputModeUIOnly InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				CurrentPlayerController->SetInputMode(InputMode);
				CurrentPlayerController->bShowMouseCursor = true;
			}
		}
		
		//열린 위젯인지 체크용
		OpenedWidgets.Add(InWidgetType, Widget);
	}

	return Widget;
}

void UUISubSystem::HideWidget(EWidgetType InWidgetType)
{
	if (CreatedWidgets.Contains(InWidgetType))
	{
		if (CreatedWidgets[InWidgetType]->Implements<UOpenable>())
		{
			IOpenable::Execute_CloseWidget(CreatedWidgets[InWidgetType]);
		}
		else
		{
			CreatedWidgets[InWidgetType]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (OpenedWidgets.Contains(InWidgetType))
	{
		OpenedWidgets.Remove(InWidgetType);

		if (OpenedWidgets.Num() == 0)
		{
			FInputModeGameOnly InputMode;
			CurrentPlayerController->bShowMouseCursor = false;
			CurrentPlayerController->SetInputMode(InputMode);
		}
	}
}

bool UUISubSystem::IsOpenedWidget(EWidgetType InWidgetType)
{
	return OpenedWidgets.Contains(InWidgetType);
}

void UUISubSystem::HideAllWiget()
{
	for (auto opened : OpenedWidgets)
	{
		HideWidget(opened.Key);
	}
}
