// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "TestInteractableActor.generated.h"

UCLASS()
class ATestInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestInteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	void OnInteractionStarted_Implementation();

	UFUNCTION(BlueprintCallable)
	void OnInteractionEnded_Implementation();

	UFUNCTION(BlueprintCallable)
	void OnPickup_Implementation();

private:
	const ECollisionChannel InteractableChannel = ECC_GameTraceChannel1;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<class UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UWidgetComponent> PopupWidget = nullptr;

};
