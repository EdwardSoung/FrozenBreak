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
			IInteractable::Execute_OnSelect(CurrentInteractionActor, true);
			UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *InteractionHitResult.GetActor()->GetName());

		}
		LastInteractionActor = CurrentInteractionActor;
		bIsInteracting = true;
	}
	else
	{
		if (bIsInteracting)
		{
			IInteractable::Execute_OnSelect(LastInteractionActor, false);
			UE_LOG(LogTemp, Log, TEXT("Hit Actor End"));

			CurrentInteractionActor = nullptr;
			LastInteractionActor = nullptr;
		}

		bIsInteracting = false;
	}
}

void UInteractionComponent::DoAction_Implementation()
{
	// 플레이어가 인터렉션을 눌렀음 -> 눌렀다는걸 인터렉션 컴포넌트한테 알려줌 -> 
	// 인터렉션 컴포넌트는 플레이어가 인터렉션을 눌렀을 때 CurrentInteractionActor가 있는지 확인, 없으면 리턴 ->
	// 있다면 LastInteractionActor, 플레이어(애니메이션, ??), 인벤토리 컴포넌트에게(얘한테는 CurrentInteractionActor를 추가로) 뭔가를 발송 (델리게이트나 인터페이스)

	if (CurrentInteractionActor)
	{
		// LastInteractionActor 가 있다.
		IInteractable::Execute_DoAction(LastInteractionActor);
		CurrentInteractionActor = nullptr;
		LastInteractionActor = nullptr;
		bIsInteracting = false;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("바라보고 있는 액터는 상호작용 불가능"));
	}

}

