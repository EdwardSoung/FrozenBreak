// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Enums.h"
#include "Data/ItemDataList.h"
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
	void AddItem(EItemType Type, int32 Amount);
	UFUNCTION()
	void TrashItem(UInventoryItem* InItem);

	UFUNCTION()
	void SendInventoryItems();

	void RefreshWeight();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UItemDataList> DataList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Bag")
	TObjectPtr<UItemData> BagData;

private:
	TArray<UInventoryItem*> Items;

	float InventoryMaxWeight = 50.0f;
	float CurrentWeight = 0.0f;
};
