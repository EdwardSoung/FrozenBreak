// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FROZENBREAK_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void OnInteractionStarted();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void OnInteractionEnded();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void TryPickup(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void OnPickup();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void TryChop(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void TryMining(AActor* Instigator);

	// 플레이어가 TryUse를 Execute 한다면 -> InteractionComponent에 Implementation 하고 ->
	// InteractionComponent는 바라보고 있는 액터가 뭔지 알고 있으니 -> 플레이어에게 그 액터를 알려주면 ->
	// 플레이어는 그 액터에 맞는 이벤트를 발생 ???

};
