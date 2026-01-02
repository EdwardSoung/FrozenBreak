// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Enums.h"
#include "Data/ItemData.h"
#include "InventoryComponent.generated.h"

class UInventoryItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROZENBREAK_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UInventoryItem* GetItem(EItemType Type);

	UFUNCTION()
	void InitInventoryUI();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION()
	void AddItem(EItemType InType, int32 InAmount, float InDurability);
	UFUNCTION()
	void MoveItem(UInventoryItem* InItem);

	UFUNCTION()
	void DropItem(UInventoryItem* InItem);

	UFUNCTION()
	void SendInventoryItems();

	void RemoveItem(UInventoryItem* InItem);
	void UseItem(int32 UID);
	void RefreshWeight();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Bag")
	TObjectPtr<UItemData> BagData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ABuffableWorldProp> CampfireClass = nullptr;

	UFUNCTION()
	void SendRawMeetData();

	UFUNCTION()
	void UseInventoryItem(UInventoryItem* InItem);
	UFUNCTION()
	void QuickSlotExecute(int32 InSlotNum);
	UFUNCTION()
	void ResetQuickSlot(int32 InSlotNum);
	UFUNCTION()
	void UpdateItem(UInventoryItem* InItem);

private:
	void SpawnCampfire();

private:
	TArray<UInventoryItem*> Items;

	float InventoryMaxWeight = 50.0f;
	float CurrentWeight = 0.0f;
};
