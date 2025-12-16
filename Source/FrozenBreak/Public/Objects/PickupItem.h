// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/ItemData.h"
#include "PickupItem.generated.h"

UCLASS()
class FROZENBREAK_API APickupItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Data")
	TObjectPtr<class UStatComponent> StatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Data")
	TObjectPtr<UItemData> Data;
};
