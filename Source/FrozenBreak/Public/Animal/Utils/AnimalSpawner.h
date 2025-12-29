// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Enums.h"
#include "AnimalSpawner.generated.h"

UCLASS()
class FROZENBREAK_API AAnimalSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAnimalSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SpawnWildAnimal();

private:

public:

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Animal")
	TSubclassOf<class AAnimalBase> AnimalToSpawn = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|AnimalType")
	EAnimalType AnimalType = EAnimalType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|AnimalCount")
	int32 MaxAnimalNumber = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Range")
	float SpawnRangeRadius = 1000.0f;
};
