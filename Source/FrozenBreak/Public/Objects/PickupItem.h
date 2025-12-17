// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/ItemData.h"
#include "Interface/Interactable.h"
#include "PickupItem.generated.h"

UCLASS()
class FROZENBREAK_API APickupItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void DoAction_Implementation() override;
	
	UFUNCTION(BlueprintCallable)
	virtual void OnSelect_Implementation(bool bIsStarted) override;

private:
	const ECollisionChannel InteractableActorChannel = ECollisionChannel::ECC_GameTraceChannel1;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Stat")
	TObjectPtr<class UStatComponent> StatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Data")
	TObjectPtr<class UItemData> Data;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Mesh")
	TObjectPtr<class UStaticMeshComponent> Mesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Root")
	TObjectPtr<class USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Widget")
	TObjectPtr<class UWidgetComponent> InteractionWidget = nullptr;
};
