// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/CraftInventoryComponent.h"
#include <GameSystem/EventSubSystem.h>
#include "Objects/InventoryItem.h"

UCraftInventoryComponent::UCraftInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCraftInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadRecipesFromTable();
	BuildRecipeIndex();
	InitializeRecipeStates();
	RebuildCraftableCachesAndBroadcastIfChanged(); // 초기 알림

	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Chraracter.OnGetPickupItem.AddDynamic(this, &UCraftInventoryComponent::OnInventoryItemAdded);
		EventSystem->Chraracter.OnTrashItem.AddDynamic(this, &UCraftInventoryComponent::OnInventoryItemRemoved);
	}
}

UInventoryItem* UCraftInventoryComponent::GetItem(EItemType Type)
{
	for (auto Item : CraftableItems)
	{
		if (Item->GetData()->ItemType == Type)
		{
			return Item;
		}
	}

	return nullptr;
}

void UCraftInventoryComponent::LoadRecipesFromTable()
{
	Recipes.Reset();

	if (!RecipeTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("RecipeTable is null"));
		return;
	}

	TArray<FCraftingRecipeRow*> Rows;
	RecipeTable->GetAllRows(TEXT("CraftRecipes"), Rows);

	for (FCraftingRecipeRow* Row : Rows)
	{
		if (!Row) continue;
		Recipes.Add(*Row);
	}

	RecipeStates.SetNum(Recipes.Num());
}

void UCraftInventoryComponent::BuildRecipeIndex()
{
	ItemToRecipes.Reset();

	for (int32 RecipeIdx = 0; RecipeIdx < Recipes.Num(); ++RecipeIdx)
	{
		for (const FRecipeIngredient& Req : Recipes[RecipeIdx].Required)
		{
			ItemToRecipes.FindOrAdd(Req.ItemType).Add(RecipeIdx);
		}
	}
}

int32 UCraftInventoryComponent::GetHave(EItemType Type) const
{
	if (const int32* Found = ItemCounts.Find(Type))
		return *Found;
	return 0;
}

int32 UCraftInventoryComponent::CalcCraftableTimesForRecipe(const FCraftingRecipeRow& R)
{
	int32 Times = INT32_MAX;

	for (const FRecipeIngredient& Req : R.Required)
	{
		if (Req.Count <= 0) continue;

		const int32 Have = GetHave(Req.ItemType);
		Times = FMath::Min(Times, Have / Req.Count);
	}

	if (Times == INT32_MAX) return 0; // 재료가 비정상적으로 비어있을 때 보호
	return Times;
}

bool UCraftInventoryComponent::AreMapsEqual(const TMap<EItemType, int32>& A, const TMap<EItemType, int32>& B)
{
	if (A.Num() != B.Num()) return false;
	for (const auto& KVP : A)
	{
		const int32* Found = B.Find(KVP.Key);
		if (!Found || *Found != KVP.Value) return false;
	}
	return true;
}

void UCraftInventoryComponent::SetCraftableItemCount(EItemType Type, int32 NewCount)
{
	if (NewCount < 0) NewCount = 0;

	UInventoryItem* Item = GetItem(Type);
	if (Item)
	{
		Item->SetAmount(NewCount); // AddAmount 말고 SetAmount 필요
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Chraracter.OnUpdateCraftItem.Broadcast(Type);
			// 더 좋게는 OnUpdateCraftItem(Type, NewCount) 형태 추천
		}
		return;
	}

	// 없으면 새로 생성
	UInventoryItem* NewItem = NewObject<UInventoryItem>(this);
	auto NewDataPtr = DataList->ItemAssetData.Find(Type);
	if (!NewDataPtr) return;

	auto NewData = NewDataPtr->Get();
	if (!NewData) return;

	NewItem->Initialize(NewData);
	NewItem->SetAmount(NewCount);

	CraftableItems.Add(NewItem);

	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Chraracter.OnAddItemToCraftInventoryUI.Broadcast(NewItem);
	}
}

void UCraftInventoryComponent::RemoveNonCraftablesFromUI()
{
	// 1) 현재 craftable 결과 아이템 타입 집합 만들기
	TSet<EItemType> CurrentCraftableTypes;

	for (const auto& KVP : CraftableTimesCrafting)
	{
		if (KVP.Value > 0) CurrentCraftableTypes.Add(KVP.Key);
	}
	for (const auto& KVP : CraftableTimesCooking)
	{
		if (KVP.Value > 0) CurrentCraftableTypes.Add(KVP.Key);
	}

	// 2) CraftableItems(=UI 대상)에서 craftable 아닌 것 제거
	for (int32 i = CraftableItems.Num() - 1; i >= 0; --i)
	{
		UInventoryItem* Item = CraftableItems[i];
		if (!Item)
		{
			CraftableItems.RemoveAtSwap(i);
			continue;
		}

		const EItemType Type = Item->GetData()->ItemType;

		if (!CurrentCraftableTypes.Contains(Type))
		{
			// UI에 제거 알림
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Chraracter.OnRemoveCraftItem.Broadcast(Item);
			}

			// 배열에서 제거
			CraftableItems.RemoveAtSwap(i);

			UE_LOG(LogTemp, Log, TEXT("[CraftInventory] Removed non-craftable item from UI. Type=%d"), (int32)Type);
		}
	}
}

void UCraftInventoryComponent::InitializeRecipeStates()
{
	CraftingSet.Reset();
	CookingSet.Reset();

	for (int32 i = 0; i < Recipes.Num(); ++i)
	{
		FRecipeState& S = RecipeStates[i];
		S.MissingKinds = 0;
		S.MissingCount.Reset();

		for (const FRecipeIngredient& Req : Recipes[i].Required)
		{
			const int32 Have = GetHave(Req.ItemType);
			const int32 Missing = FMath::Max(Req.Count - Have, 0);

			S.MissingCount.Add(Req.ItemType, Missing);
			if (Missing > 0) S.MissingKinds++;
		}

		S.bCraftable = (S.MissingKinds == 0);

		if (S.bCraftable)
		{
			if (Recipes[i].Category == ERecipeCategory::Crafting) CraftingSet.Add(Recipes[i].ResultItemType);
			else CookingSet.Add(Recipes[i].ResultItemType);
		}
	}
}

void UCraftInventoryComponent::UpdateRecipesForItem(EItemType InType)
{
	const TArray<int32>* Affected = ItemToRecipes.Find(InType);
	if (!Affected) return;

	bool bAnyCraftableFlagChanged = false;

	for (int32 RecipeIdx : *Affected)
	{
		if (!Recipes.IsValidIndex(RecipeIdx)) continue;

		const FCraftingRecipeRow& R = Recipes[RecipeIdx];
		FRecipeState& S = RecipeStates[RecipeIdx];

		int32 Need = 0;
		for (const FRecipeIngredient& Req : R.Required)
		{
			if (Req.ItemType == InType)
			{
				Need = Req.Count;
				break;
			}
		}

		const int32 Have = GetHave(InType);
		const int32 NewMissing = FMath::Max(Need - Have, 0);

		const int32 PrevMissing = S.MissingCount.FindRef(InType);

		if (PrevMissing == 0 && NewMissing > 0) S.MissingKinds++;
		if (PrevMissing > 0 && NewMissing == 0) S.MissingKinds--;

		S.MissingCount.Add(InType, NewMissing);

		const bool bWasCraftable = S.bCraftable;
		S.bCraftable = (S.MissingKinds == 0);

		if (bWasCraftable != S.bCraftable)
		{
			bAnyCraftableFlagChanged = true;

			TSet<EItemType>& SetRef = (R.Category == ERecipeCategory::Crafting) ? CraftingSet : CookingSet;
			if (S.bCraftable) SetRef.Add(R.ResultItemType);
			else              SetRef.Remove(R.ResultItemType);
		}
	}

	// craftable 플래그가 안 바뀌어도 “횟수”가 바뀔 수 있으니 리빌드
	// 역인덱스(Affected) 기반이라 여기서 한번 더 해도 충분히 가벼움
	RebuildCraftableCachesAndBroadcastIfChanged();
}

void UCraftInventoryComponent::RebuildCraftableCachesAndBroadcastIfChanged()
{
	auto BuildSortedArray = [](const TSet<EItemType>& Set, TArray<EItemType>& Out)
		{
			Out.Reset();
			Out.Reserve(Set.Num());
			for (EItemType T : Set) Out.Add(T);
			Out.Sort([](EItemType A, EItemType B) { return (uint8)A < (uint8)B; });
		};

	BuildSortedArray(CraftingSet, CraftablesCrafting);
	BuildSortedArray(CookingSet, CraftablesCooking);

	// ===== 추가: craftable 횟수 맵 재계산 =====
	CraftableTimesCrafting.Reset();
	CraftableTimesCooking.Reset();

	for (int32 i = 0; i < Recipes.Num(); ++i)
	{
		if (!RecipeStates.IsValidIndex(i)) continue;
		if (!RecipeStates[i].bCraftable) continue;

		const FCraftingRecipeRow& R = Recipes[i];
		const int32 Times = CalcCraftableTimesForRecipe(R);

		TMap<EItemType, int32>& MapRef = (R.Category == ERecipeCategory::Crafting)
			? CraftableTimesCrafting
			: CraftableTimesCooking;

		// 같은 결과 아이템을 만드는 레시피가 여러 개면 "최대 가능횟수"를 표시
		int32& Cur = MapRef.FindOrAdd(R.ResultItemType);
		Cur = FMath::Max(Cur, Times);
	}

	const bool bListChanged =
		(CraftablesCrafting != LastCraftablesCrafting) ||
		(CraftablesCooking != LastCraftablesCooking);

	const bool bCountChanged =
		(!AreMapsEqual(CraftableTimesCrafting, LastCraftableTimesCrafting)) ||
		(!AreMapsEqual(CraftableTimesCooking, LastCraftableTimesCooking));

	if (bListChanged || bCountChanged)
	{
		LastCraftablesCrafting = CraftablesCrafting;
		LastCraftablesCooking = CraftablesCooking;

		LastCraftableTimesCrafting = CraftableTimesCrafting;
		LastCraftableTimesCooking = CraftableTimesCooking;

		UE_LOG(LogTemp, Log, TEXT("UCraftInventoryComponent::OnCraftablesChanged.Broadcast (List=%d Count=%d)"),
			bListChanged ? 1 : 0, bCountChanged ? 1 : 0);

		OnCraftablesChanged.Broadcast(CraftablesCrafting, CraftablesCooking);

		// ===== UI 갱신: 절대값(가능횟수)로 세팅 =====
		for (const auto& KVP : CraftableTimesCrafting)
		{
			UE_LOG(LogTemp, Log, TEXT("Crafting Craftable: %d Times=%d"), (int32)KVP.Key, KVP.Value);
			SetCraftableItemCount(KVP.Key, KVP.Value); // ✅ 누적이 아니라 세팅
		}

		for (const auto& KVP : CraftableTimesCooking)
		{
			UE_LOG(LogTemp, Log, TEXT("Cooking  Craftable: %d Times=%d"), (int32)KVP.Key, KVP.Value);
			SetCraftableItemCount(KVP.Key, KVP.Value);
		}

		// 더 이상 craftable 아닌 UI 아이템 제거까지 하고 싶으면
		 RemoveNonCraftablesFromUI();
	}
}

void UCraftInventoryComponent::OnInventoryItemAdded(EItemType Type, int32 Amount)
{
	if (Amount <= 0) return;
	ItemCounts.FindOrAdd(Type) += Amount;
	UpdateRecipesForItem(Type);
}

void UCraftInventoryComponent::OnInventoryItemRemoved(UInventoryItem* InItem)
{
	if (!InItem) return;

	const EItemType Type = InItem->GetData()->ItemType;
	const int32 Amount = InItem->GetAmount();

	if (Amount <= 0) return;

	int32& Count = ItemCounts.FindOrAdd(Type);
	Count = FMath::Max(0, Count - Amount);

	UpdateRecipesForItem(Type);
}