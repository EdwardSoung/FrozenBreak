// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameSystem/Events/CharacterEvents.h"
#include "GameSystem/Events/StatusEvents.h"
#include "EventSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UEventSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	static UEventSubSystem* Get(const UObject* WorldContextObject);

public:
	UPROPERTY(BlueprintReadOnly, Category = "Events")
	FCharacterEvents Chraracter;

	UPROPERTY(BlueprintReadOnly, Category = "Events")
	FStatusEvents Status;

public:
	//임시..
	UFUNCTION(BlueprintCallable)
	void TestAddItem(EItemType Type, int32 Amount)
	{
		Chraracter.OnGetPickupItem.Broadcast(Type, Amount);
	}
};
