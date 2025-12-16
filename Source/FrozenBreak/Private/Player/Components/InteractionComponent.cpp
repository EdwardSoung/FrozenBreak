// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/InteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "Interface/Interactable.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.05f; // 틱 도는 빈도를 1초에 20번 정도로 제한

}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	ComponentOwner = GetOwner();
	Camera = ComponentOwner->FindComponentByClass<UCameraComponent>();
	if (ComponentOwner && Camera)
	{

	}
	else
	{
		// 이런 일이 있을 수가 있나
		return;
	}

}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 카메라의 ForwardVector 기준으로 라인 트레이싱 (화면 중앙)
	const FVector CameraLocation = Camera->GetComponentLocation();
	const FVector CameraForward = Camera->GetForwardVector();
	const FVector TargetLocation = CameraLocation + (CameraForward * InteractionDistance);

	const bool bHit = GetWorld()->LineTraceSingleByChannel
	(
		InteractionHitResult,		// 라인에 블록된 액터
		CameraLocation,
		TargetLocation,
		InteractableActorChannel	// InteractableActorChannel이 Block으로 되어있는 액터만
	);

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), CameraLocation, TargetLocation, FColor::Green, false, -1.f, 0, 0.25f);
#endif

	CurrentInteractionActor = InteractionHitResult.GetActor();

	if (bHit)
	{
		if (!bIsInteracting)
		{
			if (CurrentInteractionActor)
			{
				IInteractable::Execute_OnSelect(CurrentInteractionActor, true);
				UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *InteractionHitResult.GetActor()->GetName());
				bIsInteracting = true;
			}
		}
		else if (CurrentInteractionActor != LastInteractionActor)
		{
			if (LastInteractionActor)
			{
				IInteractable::Execute_OnSelect(LastInteractionActor, false);
				UE_LOG(LogTemp, Log, TEXT("Hit Actor End (Switch): %s"), *LastInteractionActor->GetName());
			}
			if (CurrentInteractionActor)
			{
				IInteractable::Execute_OnSelect(CurrentInteractionActor, true);
				UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *CurrentInteractionActor->GetName());
			}
		}
		LastInteractionActor = CurrentInteractionActor;

	}
	else
	{
		if (bIsInteracting)
		{
			if (LastInteractionActor)
			{
				IInteractable::Execute_OnSelect(LastInteractionActor, false);
				UE_LOG(LogTemp, Log, TEXT("Hit Actor End"));
			}
			CurrentInteractionActor = nullptr;
			LastInteractionActor = nullptr;
			bIsInteracting = false;
		}
	}
}

void UInteractionComponent::DoAction_Implementation()
{
	bool bIsTryAction = false;

	if (bIsInteracting)		// 상호작용 가능한 액터를 바라보고 있을 때만
	{
		if (CurrentInteractionActor)
		{
			IInteractable::Execute_DoAction(LastInteractionActor);

			CurrentInteractionActor = nullptr;
			LastInteractionActor = nullptr;
			bIsInteracting = false;
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("대상이 없거나 상호작용을 할 수 없는 대상"));
			return;
		}
	}
}

