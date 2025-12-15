// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "Data/Enums.h"
#include "ItemBase.generated.h"



UCLASS()
class FROZENBREAK_API AItemBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void OnSelect_Implementation(bool bIsStart);

	UFUNCTION(BlueprintCallable)
	void DoAction_Implementation();

private:
	const ECollisionChannel InteractableActorChannel = ECollisionChannel::ECC_GameTraceChannel1;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<class UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UWidgetComponent> PopupWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	TSoftObjectPtr<class UItemDataAssetBase> ItemDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PropData")
	TSoftObjectPtr<class UPropData> PropDataAsset;

};
