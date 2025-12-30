// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Damageable.h"
#include "Interface/Interactable.h"
#include "EscapeProp.generated.h"

UCLASS()
class FROZENBREAK_API AEscapeProp : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEscapeProp();

public:
	UFUNCTION(BlueprintPure, Category = "Prop|Data")
	EPropType GetPropType() const;

	UFUNCTION(BlueprintPure, Category = "Prop|Data")
	EItemType GetInteractableToolType() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> MeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Data")
	TObjectPtr<class UPropData> Data;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop|Widget")
	TObjectPtr<class UWidgetComponent> InteractionWidget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop|Widget")
	TObjectPtr<class UWidgetComponent> DurabilityWidget = nullptr;

	const ECollisionChannel InteractableActorChannel = ECollisionChannel::ECC_GameTraceChannel1;
public:
	void RockAction();

	UFUNCTION(BlueprintCallable)
	void DoAction_Implementation() override ;
	UFUNCTION(BlueprintCallable)
	void OnSelect_Implementation(bool bIsStart) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Data")
	float FatigueCostPerWork = -50.0f;

	float ToolAtkPower = 0.f;

	float MaxDurability = 0.0f;
	float CurrentDurability = 0.0f;

};
