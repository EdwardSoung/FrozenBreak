// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TestInteractionCharactor.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Components/InteractionComponent.h"
#include "PlayerComponents/InventoryComponent.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"

#include "Interface/Interactable.h"
#include "Interface/UI/Openable.h"

#include "UI/Player/InventoryWidget.h"


// Sets default values
ATestInteractionCharactor::ATestInteractionCharactor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 350.0f;
	SpringArm->SocketOffset = FVector(0, 0, 250);
	SpringArm->bUsePawnControlRotation = true;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(SpringArm);
	PlayerCamera->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

}

// Called when the game starts or when spawned
void ATestInteractionCharactor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestInteractionCharactor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestInteractionCharactor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhanced = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhanced)
	{
		enhanced->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATestInteractionCharactor::OnMoveInput);
		enhanced->BindAction(IA_Interaction, ETriggerEvent::Started, this, &ATestInteractionCharactor::OnInteractionInput);
		enhanced->BindAction(IA_Inventory, ETriggerEvent::Started, this, &ATestInteractionCharactor::OnInventoryInput);
		
	}
}

void ATestInteractionCharactor::OnMoveInput(const FInputActionValue& InValue)
{
	FVector2D inputDirection = InValue.Get<FVector2D>();
	FVector moveDirection(inputDirection.Y, inputDirection.X, 0.0f);

	FQuat controlYawRotation = FQuat(FRotator(0, GetControlRotation().Yaw, 0));
	moveDirection = controlYawRotation.RotateVector(moveDirection);

	AddMovementInput(moveDirection);
}

void ATestInteractionCharactor::OnInteractionInput(const FInputActionValue& Invale)
{
	UE_LOG(LogTemp, Log, TEXT("Player가 InteractionComponent로 인터페이스 전달"));
	IInteractable::Execute_DoAction(InteractionComponent);
}

void ATestInteractionCharactor::OnInventoryInput(const FInputActionValue& Invale)
{
	bool bIsInventoryOpen = false;

	//if (InventoryWidget)
	//{
	//	if (!bIsInventoryOpen)
	//	{
	//		IOpenable::Execute_OpenWidget(InventoryWidget);
	//		bIsInventoryOpen = true;
	//	}
	//	else
	//	{
	//		IOpenable::Execute_CloseWidget(InventoryWidget);
	//		bIsInventoryOpen = false;
	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Log, TEXT("인벤토리 위젯이 설정되지 않음"));
	//}
}

