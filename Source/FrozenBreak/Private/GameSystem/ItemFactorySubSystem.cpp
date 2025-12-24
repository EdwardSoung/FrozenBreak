// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/ItemFactorySubSystem.h"
#include "Objects/PickupItem.h"
#include "Tools/ToolActor.h"
#include "GameSystem/GameManager.h"

UItemFactorySubSystem* UItemFactorySubSystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (const UWorld* World = WorldContextObject->GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				return GameInstance->GetSubsystem<UItemFactorySubSystem>();
			}
		}
	}
	return nullptr;
}

APickupItem* UItemFactorySubSystem::Spawn(EItemType InType, FVector Location, int32 Amount, float InDurability)
{
	if (UGameManager* Manager = UGameManager::Get(this))
	{
		auto spawnClass = Manager->GetPickupItemClass(InType);
		APickupItem* DropItem = GetWorld()->SpawnActor<APickupItem>(spawnClass, Location, FRotator::ZeroRotator);
		DropItem->SetExistValues(Amount, InDurability);

		return DropItem;		
	}

	return nullptr;
}

UInventoryItem* UItemFactorySubSystem::Spawn(EItemType InType, int32 Amount, float InDurability)
{
	if (UGameManager* Manager = UGameManager::Get(this))
	{
		auto NewItem = NewObject<UInventoryItem>(this);
		auto NewData = Manager->GetItemData(InType);
		NewItem->Initialize(Manager->GetUID(), NewData, InDurability);
		NewItem->AddAmount(Amount);

		return NewItem;
	}
	return nullptr;
}

AToolActor* UItemFactorySubSystem::SpawnTool(EItemType InType, float InDurability)
{
	if (UGameManager* Manager = UGameManager::Get(this))
	{
		auto spawnClass = Manager->GetToolClass(InType);

		if (spawnClass)
		{
			AToolActor* EquipItem = GetWorld()->SpawnActor<AToolActor>(spawnClass);
			
			auto NewData = Manager->GetItemData(InType);
			EquipItem->InitializeData(NewData, InDurability);

			return EquipItem;
		}
	}

	return nullptr;
}
