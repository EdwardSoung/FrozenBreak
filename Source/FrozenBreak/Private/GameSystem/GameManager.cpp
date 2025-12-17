// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/GameManager.h"
#include "Data/WidgetDataTable.h"
#include "Data/ItemObjectTableData.h"

UGameManager* UGameManager::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    if (UWorld* World = WorldContextObject->GetWorld())
    {
        return Cast<UGameManager>(World->GetGameInstance());
    }

    return nullptr;
}

TSubclassOf<UUserWidget> UGameManager::GetWidgetClass(EWidgetType InType)
{
    TArray<FWidgetDataTable*> Rows;
    WidgetData->GetAllRows(TEXT("SpawnInit"), Rows);

    for (FWidgetDataTable* row : Rows)
    {
        if (row->Type == InType)
        {
            return row->WidgetClass;
        }
    }
    return nullptr;
}

TSubclassOf<APickupItem> UGameManager::GetPickupItemClass(EItemType InType)
{
    TArray<FItemObjectTableData*> Rows;
    PickupItemTable->GetAllRows(TEXT("SpawnInit"), Rows);

    for (FItemObjectTableData* row : Rows)
    {
        if (row->Type == InType)
        {
            return row->ItemClass;
        }
    }
    return nullptr;
}
