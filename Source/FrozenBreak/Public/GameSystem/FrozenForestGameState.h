// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/Enums.h"
#include "FrozenForestGameState.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API AFrozenForestGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AFrozenForestGameState();	

public:
	void SetGameState(EGameState InState);

protected:
	UFUNCTION()
	void OnGameStateChanged(EGameState InState);
};
