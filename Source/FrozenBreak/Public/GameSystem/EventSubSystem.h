// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameSystem/Events/CharacterEvents.h"
#include "GameSystem/Events/StatusEvents.h"
#include "GameSystem/Events/UIEvents.h"
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
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void ResetSystem();

private:
	void OnLevelLoaded(UWorld* World);

public:
	UPROPERTY(BlueprintReadOnly, Category = "Events")
	FCharacterEvents Character;

	UPROPERTY(BlueprintReadOnly, Category = "Events")
	FStatusEvents Status;

	UPROPERTY(BlueprintReadOnly, Category = "Events")
	FUIEvents UI;

public:
	//임시..
	UFUNCTION(BlueprintCallable)
	void TestAddItem(EItemType Type, int32 Amount)
	{
		Character.OnGetPickupItem.Broadcast(Type, Amount, 0);
	}
};
