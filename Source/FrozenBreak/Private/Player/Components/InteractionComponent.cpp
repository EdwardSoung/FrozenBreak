// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/InteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "Interface/Interactable.h"
#include "Objects/WorldProp.h"
#include "Objects/PickupItem.h"
#include "Objects/EscapeProp.h"

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
		UE_LOG(LogTemp, Warning, TEXT("플레이어 혹은 카메라 컴포넌트가 없습니다"));
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

	const float SweepRadius = 30.0f; // 값 키우면 더 잘 맞음

	const bool bHit = GetWorld()->SweepSingleByChannel(
		InteractionHitResult,
		CameraLocation,
		TargetLocation,
		FQuat::Identity,
		InteractableActorChannel,
		FCollisionShape::MakeSphere(SweepRadius)
	);

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), CameraLocation, TargetLocation, FColor::Green, false, -1.f, 0, 0.25f);
	DrawDebugSphere(GetWorld(), TargetLocation, SweepRadius, 12, FColor::Yellow, false, 0.05f);
#endif

	CurrentInteractionActor = InteractionHitResult.GetActor();

	// 뭔가 라인에 맞았다 (상호작용 가능한 무언가)
	if (bHit && CurrentInteractionActor)
	{
		// 플레이어와 상호작용 가능 액터와의 거리 계산
		FVector PlayerLocation = GetOwner()->GetActorLocation();
		FVector InteractionActorLoaction = CurrentInteractionActor->GetActorLocation();
		BetweenDistance = FVector::Distance(PlayerLocation, InteractionActorLoaction);

		if (CheckEscapeTarget())
			ActivateInteractDistance = 2500.0f;
		else
			ActivateInteractDistance = 250.0f;

		// 지금 상호작용 하고있지 않다
		if (!bIsInteracting)
		{
			// 플레이어와 상호작용 할 액터의 거리가 ActivateInteractDistance 이하이면
			if (BetweenDistance <= ActivateInteractDistance)
			{
				// 바라본 대상이 WorldProp / Item 인지 구분해주는 함수
				ProcessInteractableTarget();
			}
			else
			{
				// 범위 밖이면 표시하지 않음
				if (LastInteractionActor)
				{
					IInteractable::Execute_OnSelect(LastInteractionActor, false);
					LastInteractionActor = nullptr;
				}
			}
		}
		// 바라보고 있는 액터와 마지막으로 바라본 액터가 다르다면
		else if (CurrentInteractionActor != LastInteractionActor)
		{
			// 마지막으로 바라봤던 액터에게는
			if (LastInteractionActor)
			{
				// "너 이제 안 보고 있어" 라고 알림
				IInteractable::Execute_OnSelect(LastInteractionActor, false);
				UE_LOG(LogTemp, Log, TEXT("Hit Actor End (Switch): %s"), *LastInteractionActor->GetName());
			}

			// 스위치 대상이 범위 내일 때만 새로 표시
			if (BetweenDistance <= ActivateInteractDistance)
			{
				// 바라본 대상이 WorldProp / Item 인지 구분해주는 함수
				ProcessInteractableTarget();
			}
			// 범위 밖이면 상호작용 종료
			else
			{
				bIsInteracting = false;
			}
		}

		if (bIsInteracting && (BetweenDistance > ActivateInteractDistance))
		{
			IInteractable::Execute_OnSelect(CurrentInteractionActor, false);
			UE_LOG(LogTemp, Log, TEXT("거리가 멀어져 위젯 닫음. 거리 : %.1f"), ActivateInteractDistance);
			bIsInteracting = false;
			LastInteractionActor = nullptr;
		}

		LastInteractionActor = CurrentInteractionActor;
	}
	// 라인에 맞은게 없다
	else
	{
		// 상호작용 하고 있었다면
		if (bIsInteracting)
		{
			// 마지막으로 바라봤었던 (진짜 방금 전까지 보고있었던) 액터에게
			if (LastInteractionActor)
			{
				// "너 이제 안 보고 있어" 알림
				IInteractable::Execute_OnSelect(LastInteractionActor, false);
				UE_LOG(LogTemp, Log, TEXT("Hit Actor End"));
			}

			// 초기화
			CurrentInteractionActor = nullptr;
			LastInteractionActor = nullptr;
			bIsInteracting = false;
			BetweenDistance = 0.f;
		}
	}
}

void UInteractionComponent::DoAction_Implementation() // 플레이어가 상호작용 키를 눌렀을 때
{
	if (bIsInteracting)		// 상호작용 가능한 액터를 바라보고 있을 때만
	{
		if (CurrentInteractionActor) // 바라보고 있는 액터에게
		{
			// "너가 할 수 있는거 하셈" 알림
			UE_LOG(LogTemp, Log, TEXT("인컴 : 인터페이스 받고 바라보고 있는 액터에게 인터페이스 보냄"));
				IInteractable::Execute_DoAction(CurrentInteractionActor);

			// 초기화
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

void UInteractionComponent::ProcessInteractableTarget()
{
	if (const AWorldProp* Prop = Cast<AWorldProp>(CurrentInteractionActor))
	{
		HitActorInteractableToolType = Prop->GetInteractableToolType();

		// 바라보고 있는 액터의 Data->InteractableToolType 이 None 아면
		if (HitActorInteractableToolType == EItemType::None || HitActorInteractableToolType == EItemType::Campfire)
		{
			// 바라보고 있는 액터에게 "너 지금 바라봐지고 있어" 라고 알림
			IInteractable::Execute_OnSelect(CurrentInteractionActor, true);
			UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *InteractionHitResult.GetActor()->GetName());
			UE_LOG(LogTemp, Warning, TEXT("이건 나무나 바위가 아닌데 Prop이긴 함"));
			bIsInteracting = true;
		}
		// 바라보고 있는 액터의 Data->InteractableToolType 이 Player의 CurrentHeldItemType과 같다면
		else if (PlayerCurrentTool == HitActorInteractableToolType)
		{
			IInteractable::Execute_OnSelect(CurrentInteractionActor, true);
			UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *InteractionHitResult.GetActor()->GetName());
			UE_LOG(LogTemp, Warning, TEXT("이건 나무나 바위임"));
			bIsInteracting = true;
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("플레이어의 도구는 바라보고 있는 액터에 상호작용 불가"));
		}
	}
	// 바라보고 있는 액터가 Item 이면
	else
	{
		IInteractable::Execute_OnSelect(CurrentInteractionActor, true);
		UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *InteractionHitResult.GetActor()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("이건 나무나 바위가 아니고, Item임"));
		bIsInteracting = true;
	}
}

bool UInteractionComponent::CheckEscapeTarget()
{
	if (const AEscapeProp* Prop = Cast<AEscapeProp>(CurrentInteractionActor))
	{
		if (PlayerCurrentTool == EItemType::Pickaxe)
		{
			return true;
		}
	}
	return false;
}

