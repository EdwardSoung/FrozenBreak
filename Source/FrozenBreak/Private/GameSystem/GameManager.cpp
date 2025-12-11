// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/GameManager.h"

UGameManager* UGameManager::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    if (UWorld* World = WorldContextObject->GetWorld())
    {
        return Cast<UGameManager>(World->GetGameInstance());
    }

    return nullptr;
}