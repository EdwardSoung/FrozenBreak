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
                // GameInstance���� ����ý����� �����ɴϴ�.
                return GameInstance->GetSubsystem<UEventSubSystem>();
            }
        }
    }
    return nullptr;
}

void UEventSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UEventSubSystem::OnLevelLoaded);
}

void UEventSubSystem::ResetSystem()
{

}
void UEventSubSystem::Deinitialize()
{
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
    Super::Deinitialize();
}

void UEventSubSystem::OnLevelLoaded(UWorld* World)
{
    ResetSystem();
}
