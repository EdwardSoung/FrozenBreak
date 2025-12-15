// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/EventSubSystem.h"

UEventSubSystem* UEventSubSystem::Get(const UObject* WorldContextObject)
{
    if (WorldContextObject)
    {
        if (const UWorld* World = WorldContextObject->GetWorld())
        {
            if (UGameInstance* GameInstance = World->GetGameInstance())
            {
                // GameInstance에서 서브시스템을 가져옵니다.
                return GameInstance->GetSubsystem<UEventSubSystem>();
            }
        }
    }
    return nullptr;
}
