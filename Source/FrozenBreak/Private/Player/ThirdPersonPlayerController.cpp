// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ThirdPersonPlayerController.h"
#include "GameSystem/UISubSystem.h"
#include "GameSystem/EventSubSystem.h"
#include "GameSystem/GameManager.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <Kismet/GameplayStatics.h>

void AThirdPersonPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
}

void AThirdPersonPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->ShowWidget(EWidgetType::HUD);
	}

	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->UI.OnToggleQuitMenu.AddDynamic(this, &AThirdPersonPlayerController::ToggleQuitMenu);
	}
}

void AThirdPersonPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// IA_Pause 바인딩
		if (IA_Pause)
			EnhancedInputComponent->BindAction(IA_Pause, ETriggerEvent::Started, this, &AThirdPersonPlayerController::OnPauseTriggered);
		
		if (IA_Inventory)		
			EnhancedInputComponent->BindAction(IA_Inventory, ETriggerEvent::Started, this, &AThirdPersonPlayerController::OnInventoryTriggered);
		
		if(IA_PlayerStatus)
			EnhancedInputComponent->BindAction(IA_PlayerStatus, ETriggerEvent::Started, this, &AThirdPersonPlayerController::OnPlayerStatusTriggered);

		//1,2,3,4,5를 여기서 하는게 맞는건지...
		//InputMode가 GameOnly라 퀵슬롯 사용하려면...
		if (IA_QuickSlot1)
			EnhancedInputComponent->BindAction(IA_QuickSlot1, ETriggerEvent::Started, this, &AThirdPersonPlayerController::OnOneStarted);
		if (IA_QuickSlot2)
			EnhancedInputComponent->BindAction(IA_QuickSlot2, ETriggerEvent::Started, this, &AThirdPersonPlayerController::OnTwoStarted);
		if (IA_QuickSlot3)
			EnhancedInputComponent->BindAction(IA_QuickSlot3, ETriggerEvent::Started, this, &AThirdPersonPlayerController::OnThreeStarted);
		if (IA_QuickSlot4)
			EnhancedInputComponent->BindAction(IA_QuickSlot4, ETriggerEvent::Started, this, &AThirdPersonPlayerController::OnFourStarted);
		if (IA_QuickSlot5)
			EnhancedInputComponent->BindAction(IA_QuickSlot5, ETriggerEvent::Started, this, &AThirdPersonPlayerController::OnFiveStarted);
	}
}

void AThirdPersonPlayerController::ToggleQuitMenu()
{
	if (bIsMenuOpen)
	{
		if (UUISubSystem* UISystem = UUISubSystem::Get(this))
		{
			UISystem->HideWidget(EWidgetType::QuitMenu);
		}

		UGameplayStatics::SetGamePaused(this, false);

		bIsMenuOpen = false;
	}
	else
	{
		bIsMenuOpen = true;
		if (UUISubSystem* UISystem = UUISubSystem::Get(this))
		{
			UISystem->ShowWidget(EWidgetType::QuitMenu);
		}
		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);
		UGameplayStatics::SetGamePaused(this, true);
	}
}

void AThirdPersonPlayerController::OnPauseTriggered(const FInputActionValue& Value)
{
	if (UGameManager* Manager = UGameManager::Get(this))
	{
		if (Manager->GetGameState() != EGameState::Playing)
			return;
	}
	ToggleQuitMenu();
}

void AThirdPersonPlayerController::OnInventoryTriggered(const FInputActionValue& Value)
{
	if (UGameManager* Manager = UGameManager::Get(this))
	{
		if (Manager->GetGameState() != EGameState::Playing)
			return;
	}
	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->ShowWidget(EWidgetType::Inventory);
	}
}

void AThirdPersonPlayerController::OnPlayerStatusTriggered(const FInputActionValue& Value)
{
	if (UGameManager* Manager = UGameManager::Get(this))
	{
		if (Manager->GetGameState() != EGameState::Playing)
			return;
	}
	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->ShowWidget(EWidgetType::CharacterState);
	}
}

void AThirdPersonPlayerController::OnOneStarted(const FInputActionValue& Value)
{
	QuickSlotExecute(1);
}

void AThirdPersonPlayerController::OnTwoStarted(const FInputActionValue& Value)
{
	QuickSlotExecute(2);
}

void AThirdPersonPlayerController::OnThreeStarted(const FInputActionValue& Value)
{
	QuickSlotExecute(3);
}

void AThirdPersonPlayerController::OnFourStarted(const FInputActionValue& Value)
{
	QuickSlotExecute(4);
}

void AThirdPersonPlayerController::OnFiveStarted(const FInputActionValue& Value)
{
	QuickSlotExecute(5);
}

void AThirdPersonPlayerController::QuickSlotExecute(int32 InSlotKey)
{
	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		if (UISystem->IsOpenedWidget(EWidgetType::Inventory) == false)
		{
			//퀵슬롯 사용
			if (UEventSubSystem* Event = UEventSubSystem::Get(this))
			{
				Event->UI.OnQuickSlotExecute.Broadcast(InSlotKey);
			}
		}

	}
}
