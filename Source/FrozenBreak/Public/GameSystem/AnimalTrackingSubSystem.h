// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/Enums.h"
#include "AnimalTrackingSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UAnimalTrackingSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	inline int32 GetCurrentAnimalNumber(EAnimalType InType)
	{
		UE_LOG(LogTemp, Log, TEXT("current animal number : %d"), AnimalCountMap[InType]);
		return AnimalCountMap[InType];
	}

	inline void SetAnimalNumberMap(EAnimalType InType)
	{
		AnimalCountMap.Add(InType) = 0;
	}

	inline void IncreaseCurrentAnimalNumber(EAnimalType InType)
	{
		AnimalCountMap[InType]++;
	}

	inline void DecreaseCurrentAnimalNumber(EAnimalType InType)
	{
		AnimalCountMap[InType]--;
	}

protected:
	UPROPERTY()
	TMap<EAnimalType, int32> AnimalCountMap;
};
