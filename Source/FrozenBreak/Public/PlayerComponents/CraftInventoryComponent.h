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

	// 요리 시작
	UFUNCTION()
	void StartCooking(UInventoryItem* InIngredient);

	// ===== Cooking 입력 재료(투입된 재료) =====
	TMap<EItemType, int32> CookingInputCounts;

	// 첫 번째 가능한 Cooking 레시피(Recipes 배열 순서 기준)
	const FCraftingRecipeRow* FindFirstCookableRecipeFromInputs() const;

	// 결과 타입으로 UInventoryItem 확보(없으면 생성)
	UInventoryItem* GetOrCreateCraftableItem(EItemType ResultType);

	// 요리 입력에서 특정 아이템 보유량 조회
	int32 GetHaveCookingInput(EItemType Type) const;

	void SetCraftProcess();
	void FinishCraft();

	const FCraftingRecipeRow* FindBestRecipeForResult(EItemType ResultType, ERecipeCategory Category) const;
	bool CanConsumeRecipeOnce(const FCraftingRecipeRow& R) const;
	void ConsumeRecipeOnce(const FCraftingRecipeRow& R);

	UFUNCTION()
	void SetCurrentFatigue(float InValue);

	inline bool HasEnoughFatigue() 
	{
		return MaxCraftCost <= CurrentFatigue;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FatigueUse = 10.0f;

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
	float CraftRate = 0.1f;

	// 작업량 : 기본 0(0일 시 작업 실패)
	float MaxCraftCost = 0.0f;

	// 현재 작업량(0 ~ MAxCraftCost)
	float CurrentCraftCost = 0.0f;

	// 작업 아이템
	TWeakObjectPtr<UInventoryItem> CurrentItemToCraft = nullptr;
	// 작업 완료 시 아이템 수량(현재 시나리오에서는 무조건 1)
	int32 CraftResultCount = 1;

	float CurrentFatigue = 0.0f;
};
