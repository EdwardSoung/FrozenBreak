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

UItemData* UGameManager::GetItemData(EItemType InType)
{
    return ItemDataList->ItemAssetData.Find(InType)->Get();
}

TSubclassOf<class AToolActor> UGameManager::GetToolClass(EItemType InType)
{
    return EquipItemData->ToolData.Find(InType)->Get();
}

uint32 UGameManager::GetUID()
{
    return CurrentUID++;
}
