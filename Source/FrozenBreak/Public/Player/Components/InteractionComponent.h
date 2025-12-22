// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/Interactable.h"
#include "Data/Enums.h"
#include "InteractionComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FROZENBREAK_API UInteractionComponent : public UActorComponent, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DoAction_Implementation();

	inline void SetPlayerCurrentTool(EItemType InType) { PlayerCurrentTool = InType; }


private:
	// 라인 충돌지점에 뭐가 충돌했는지 알려줌
	FHitResult InteractionHitResult;

	// 트레이스 채널, 이 트레이스 채널이 블록된 액터만 상호작용 가능 (일반 액터의 트레이스 방지)
	const ECollisionChannel InteractableActorChannel = ECollisionChannel::ECC_GameTraceChannel1;

	const AActor* ComponentOwner = nullptr;

	// 현재 트레이싱 되고있는 액터
	AActor* CurrentInteractionActor = nullptr;

	// 마지막으로 트레이싱 된 액터를 저장
	AActor* LastInteractionActor = nullptr;

	const class UCameraComponent* Camera = nullptr;

protected:
	// 라인의 길이
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionDistance = 600.f;

	// 플레이어와 상호작용 가능 액터와의 거리가 이 수치 이하여야 상호작용 가능함
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ActivateInteractDistance = 250.f;

	// 플레이어와 상호작용 할 액터 사이의 거리
	UPROPERTY(VisibleAnywhere)
	float BetweenDistance = 0.f;

	// 한번만 실행되게 만들기 위한 bool
	bool bIsInteracting = false;

	EItemType PlayerCurrentTool = EItemType::None;

	EItemType HitActorInteractableToolType = EItemType::None;
};
