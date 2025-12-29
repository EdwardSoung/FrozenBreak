// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/Utils/AnimalSpawner.h"
#include "Animal/AnimalBase.h"
#include "GameSystem/EventSubSystem.h"
#include "GameSystem/AnimalTrackingSubSystem.h"

// Sets default values
AAnimalSpawner::AAnimalSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AAnimalSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Character.OnPlayerSleep.AddDynamic(this, &AAnimalSpawner::SpawnWildAnimal);
	}
	GetWorld()->GetSubsystem<UAnimalTrackingSubSystem>()->SetAnimalNumberMap(AnimalType);
	SpawnWildAnimal();
}

void AAnimalSpawner::SpawnWildAnimal()
{
	int32 CurrentAnimalCount = GetWorld()->GetSubsystem<UAnimalTrackingSubSystem>()->GetCurrentAnimalNumber(AnimalType);

	if (AnimalToSpawn != nullptr)
	{
		for (int i = CurrentAnimalCount; i < MaxAnimalNumber; i++)
		{
			float X = FMath::RandRange(-SpawnRangeRadius, SpawnRangeRadius);
			float Y = FMath::RandRange(-SpawnRangeRadius, SpawnRangeRadius);

			AAnimalBase* Animal = GetWorld()->SpawnActor<AAnimalBase>(AnimalToSpawn, GetActorLocation() + FVector(X, Y, 100), FRotator::ZeroRotator);

			if (AnimalType != EAnimalType::None && Animal)
			{
				Animal->SetAnimalType(AnimalType);
				GetWorld()->GetSubsystem<UAnimalTrackingSubSystem>()->IncreaseCurrentAnimalNumber(AnimalType);
			}
			else
			{
				i -= 1;
			}
		}
	}
}
