// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Enums.h"
#include "Objects/InventoryItem.h"
#include "ItemFactorySubSystem.generated.h"

class APickupItem;

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UItemFactorySubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	static UItemFactorySubSystem* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	APickupItem* Spawn(EItemType InType, FVector Location, int32 Amount = 1);

	UInventoryItem* Spawn(EItemType InType, int32 Amount = 1);

	UFUNCTION(BlueprintCallable)
	class AToolActor* SpawnTool(EItemType InType);
};
