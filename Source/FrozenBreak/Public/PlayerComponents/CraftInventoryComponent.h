// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/CraftingRecipeRow.h"
#include "Data/Enums.h"
#include "CraftInventoryComponent.generated.h"

class UDataTable;
class UInventoryItem;
class UItemDataList;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCraftablesChanged,
	const TArray<EItemType>&, CraftablesCrafting,
	const TArray<EItemType>&, CraftablesCooking
);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FROZENBREAK_API UCraftInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCraftInventoryComponent();

protected:
	virtual void BeginPlay() override;

	inline void SetCraftAmount(float InAmount) 
	{ 
		CraftAmount = DefaultCraftAmount;
		CraftAmount += InAmount;
	}

public:
	// UI 구독용 (목록 변화 알림)
	UPROPERTY(BlueprintAssignable)
	FOnCraftablesChanged OnCraftablesChanged;

	// ===== Data =====
	UPROPERTY(EditDefaultsOnly, Category = "Craft|Data")
	TObjectPtr<UDataTable> RecipeTable = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Craft|Data")
	TObjectPtr<UItemDataList> DataList = nullptr;

private:
	// ===== 전체 인벤 데이터 수신 =====
	UFUNCTION()
	void OnReceiveInventoryData(TArray<UInventoryItem*> InData);

	// ===== 계산 =====
	void LoadRecipesFromTable();
	void BuildRecipeIndex();
	void InitializeRecipeStates();
	void RecomputeAllFromItemCounts();

	int32 GetHave(EItemType Type) const;
	int32 CalcCraftableTimesForRecipe(const FCraftingRecipeRow& R);

	// ===== UI 갱신 =====
	void RebuildCraftableCachesAndBroadcastIfChanged();
	void SetCraftableItemCount(EItemType Type, int32 NewCount);
	void RemoveNonCraftablesFromUI();

	// util
	bool AreMapsEqual(const TMap<EItemType, int32>& A, const TMap<EItemType, int32>& B);
	UInventoryItem* GetItem(EItemType Type);

	// 제작 시작
	UFUNCTION()
	void StartCrafting(UInventoryItem* ItemToCraft);

	void SetCraftProcess();

private:
	// ===== 레시피 =====
	TArray<FCraftingRecipeRow> Recipes;

	struct FRecipeState
	{
		int32 MissingKinds = 0;
		TMap<EItemType, int32> MissingCount;
		bool bCraftable = false;
	};
	TArray<FRecipeState> RecipeStates;

	// 역인덱스
	TMap<EItemType, TArray<int32>> ItemToRecipes;

	// ===== 인벤 수량 (전체 인벤 기준) =====
	TMap<EItemType, int32> ItemCounts;

	// ===== Craftable 캐시 =====
	TSet<EItemType> CraftingSet;
	TSet<EItemType> CookingSet;

	TArray<EItemType> CraftablesCrafting;
	TArray<EItemType> CraftablesCooking;
	TArray<EItemType> LastCraftablesCrafting;
	TArray<EItemType> LastCraftablesCooking;

	TMap<EItemType, int32> CraftableTimesCrafting;
	TMap<EItemType, int32> CraftableTimesCooking;
	TMap<EItemType, int32> LastCraftableTimesCrafting;
	TMap<EItemType, int32> LastCraftableTimesCooking;

	// UI용 아이템
	TArray<UInventoryItem*> CraftableItems;

	// 작업력 : 기본 10
	const float DefaultCraftAmount = 10.0f;
	float CraftAmount = 10.0f;

	// 제작 타이머 관련 변수
	FTimerHandle CraftHandle;
	float CraftRate = 1.0f;

	// 작업량 : 기본 0(0일 시 작업 실패)
	float MAxCraftCost = 0.0f;

	// 현재 작업량(0 ~ MAxCraftCost)
	float CurrentCraftCost = 0.0f;

	// 작업 아이템
	TWeakObjectPtr<UInventoryItem> CurrentItemToCraft = nullptr;
	// 작업 완료 시 아이템 수량(현재 시나리오에서는 무조건 1)
	int32 CraftResultCount = 1;
};
