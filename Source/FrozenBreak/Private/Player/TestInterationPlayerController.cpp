// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/TestInterationPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubSystems.h"
#include "InputMappingContext.h"
#include "Player/TestInteractionCharactor.h"

void ATestInterationPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());	// Subsystem 가져오기 시도
	if (Subsystem)	// Subsystem이 null이 아니면
	{
		Subsystem->AddMappingContext(DefaultMappingContext, GameInputPriority);
	}
}

void ATestInterationPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* enhanced = Cast<UEnhancedInputComponent>(InputComponent);
	if (enhanced)	// 입력 컴포넌트가 향상된 입력 컴포넌트일 때
	{
		//UE_LOG(LogTemp, Log, TEXT("바인드 성공"));
		enhanced->BindAction(IA_Camera, ETriggerEvent::Triggered, this, &ATestInterationPlayerController::OnCameraInput);
	}
}

void ATestInterationPlayerController::OnCameraInput(const FInputActionValue& InValue)
{
	FVector2D lookAxis = InValue.Get<FVector2D>();
	//UE_LOG(LogTemp, Log, TEXT("OnLookInput : %s"), *lookAxis.ToString());
	AddYawInput(lookAxis.X);
	AddPitchInput(lookAxis.Y);
}
