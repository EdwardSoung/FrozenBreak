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
		// 이 컴포넌트의 주인은 라인트레이스 히트에서 제외됨
		QueryParams.AddIgnoredActor(ComponentOwner);
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
		InteractableActorChannel,	// InteractableActorChannel이 Block으로 되어있는 액터만
		QueryParams					// 제외할 것 (컴포넌트 주인)
	);

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), CameraLocation, TargetLocation, FColor::Green, false, -1.f, 0, 0.25f);
#endif

	AActor* HitActor = InteractionHitResult.GetActor();

	if (bHit)
	{
		if (!bIsInteracting)
		{
			IInteractable::Execute_OnInteractionStarted(HitActor);
			UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *InteractionHitResult.GetActor()->GetName());

		}
		LastInteractionActor = HitActor;
		bIsInteracting = true;
	}
	else
	{
		if (bIsInteracting)
		{
			IInteractable::Execute_OnInteractionEnded(LastInteractionActor);
			UE_LOG(LogTemp, Log, TEXT("Hit Actor End"));
		}


		bIsInteracting = false;
	}
}

