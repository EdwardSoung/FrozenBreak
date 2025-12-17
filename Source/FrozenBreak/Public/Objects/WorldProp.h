// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "GameSystem/Events/StatusEvents.h"
#include "WorldProp.generated.h"

UCLASS()
class FROZENBREAK_API AWorldProp : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldProp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION(BlueprintCallable)
	virtual void DoAction_Implementation(/*동작 타입을 입력받아도 좋을 것 같음...*/) override;
	//예시 : 공격인지(벌목등 포함), 획득인지, 작업인지..
	UFUNCTION(BlueprintCallable)
	virtual void OnSelect_Implementation(bool bIsStarted) override;

	void InitStat(float InAttack, float InHealth);

private:
	const ECollisionChannel InteractableActorChannel = ECollisionChannel::ECC_GameTraceChannel1;
	const float BedUsageHours = 4.0f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Stat")
	TObjectPtr<class UStatComponent> StatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Data")
	TObjectPtr<class UPropData> Data;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop|Mesh")
	TObjectPtr<class UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop|Widget")
	TObjectPtr<class UWidgetComponent> InteractionWidget = nullptr;

	// ==== 테스트 ====
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APickupItem> GenerateItemClass = nullptr;

	UPROPERTY()
	class UEventSubSystem* EventSystem = nullptr;

protected:
	// 작업시 소모시킬 피로도 (최종(확정된) 값이 아님)
	const float FatigueCostPerWork = -0.5f;

	// 침대 사용시 회복시킬 피로도 (최대치)
	const float FatigueRecoveryAmount = 100.f;

	// 침대 사용시 감소시킬 배고픔
	const float HungerReductionAmount = -30.f;
};
