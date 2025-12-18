// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Data/CraftingRecipeRow.h>
#include "Data/ItemDataList.h"
#include "CraftInventoryComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCraftablesChanged,
	const TArray<EItemType>&, CraftablesCrafting,
	const TArray<EItemType>&, CraftablesCooking
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROZENBREAK_API UCraftInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCraftInventoryComponent();

protected:
	virtual void BeginPlay() override;

	UInventoryItem* GetItem(EItemType Type);

public:
	// UI가 구독할 이벤트 (제작/요리 각각 결과 아이템 목록)
	UPROPERTY(BlueprintAssignable, Category = "Craft")
	FOnCraftablesChanged OnCraftablesChanged;

	// UI에서 현재 탭별 목록 가져가고 싶을 때
	UFUNCTION(BlueprintCallable, Category = "Craft")
	const TArray<EItemType>& GetCraftablesCrafting() const { return CraftablesCrafting; }

	UFUNCTION(BlueprintCallable, Category = "Craft")
	const TArray<EItemType>& GetCraftablesCooking() const { return CraftablesCooking; }

private:


private:
	UFUNCTION()
	void OnInventoryItemAdded(EItemType Type, int32 Amount);

	UFUNCTION()
	void OnInventoryItemRemoved(UInventoryItem* InItem);

	// init
	void LoadRecipesFromTable();
	void BuildRecipeIndex();
	void InitializeRecipeStates();

	// update
	void UpdateRecipesForItem(EItemType InType);
	void RebuildCraftableCachesAndBroadcastIfChanged();

	int32 GetHave(EItemType Type) const;

	int32 CalcCraftableTimesForRecipe(const FCraftingRecipeRow& R);
	bool AreMapsEqual(const TMap<EItemType, int32>& A, const TMap<EItemType, int32>& B);
	void SetCraftableItemCount(EItemType Type, int32 NewCount);
	void RemoveNonCraftablesFromUI();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UItemDataList> DataList;

private:
	TArray<UInventoryItem*> CraftableItems;

	// ===== 레시피 데이터 =====
	UPROPERTY(EditDefaultsOnly, Category = "Craft|Data")
	TObjectPtr<UDataTable> RecipeTable = nullptr;

	TArray<FCraftingRecipeRow> Recipes;                 // 런타임 캐시
	TMap<EItemType, TArray<int32>> ItemToRecipes;       // 역인덱스

	// ===== 인벤 수량 =====
	TMap<EItemType, int32> ItemCounts;

	// ===== 레시피 상태(MissingKinds) =====
	struct FRecipeState
	{
		int32 MissingKinds = 0;
		TMap<EItemType, int32> MissingCount; // 재료별 부족분
		bool bCraftable = false;
	};
	TArray<FRecipeState> RecipeStates;

	TArray<EItemType> CraftablesCrafting;
	TArray<EItemType> CraftablesCooking;

	TArray<EItemType> LastCraftablesCrafting;
	TArray<EItemType> LastCraftablesCooking;

	// craftable 결과 캐시(탭별) - 기존 유지
	TSet<EItemType> CraftingSet;
	TSet<EItemType> CookingSet;

	// ===== 추가: 아이템별 제작가능 횟수 캐시 =====
	TMap<EItemType, int32> CraftableTimesCrafting;
	TMap<EItemType, int32> CraftableTimesCooking;

	TMap<EItemType, int32> LastCraftableTimesCrafting;
	TMap<EItemType, int32> LastCraftableTimesCooking;
};
