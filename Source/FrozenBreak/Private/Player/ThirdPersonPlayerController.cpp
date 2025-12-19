// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ThirdPersonPlayerController.h"
#include "GameSystem/UISubSystem.h"
#include "GameSystem/EventSubSystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <Kismet/GameplayStatics.h>

void AThirdPersonPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
	ToggleQuitMenu();
}

void AThirdPersonPlayerController::OnInventoryTriggered(const FInputActionValue& Value)
{
	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->ShowWidget(EWidgetType::Inventory);
	}
}

void AThirdPersonPlayerController::OnPlayerStatusTriggered(const FInputActionValue& Value)
{
	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->ShowWidget(EWidgetType::CharacterState);
	}
}
