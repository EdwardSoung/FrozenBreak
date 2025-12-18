#include "PlayerComponents/CraftInventoryComponent.h"
#include "Objects/InventoryItem.h"
#include "GameSystem/EventSubSystem.h"
#include "Data/ItemDataList.h"

UCraftInventoryComponent::UCraftInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCraftInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadRecipesFromTable();
	BuildRecipeIndex();

	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Chraracter.OnSendInventoryData.AddDynamic(
			this, &UCraftInventoryComponent::OnReceiveInventoryData);

		EventSystem->Chraracter.OnRequestIventoryItems.Broadcast();
	}
}

// ==================== Inventory ====================

void UCraftInventoryComponent::OnReceiveInventoryData(TArray<UInventoryItem*> InData)
{
	ItemCounts.Reset();

	for (UInventoryItem* Item : InData)
	{
		if (!Item || !Item->GetData()) continue;

		const EItemType Type = Item->GetData()->ItemType;
		const int32 Amount = Item->GetAmount();

		if (Type == EItemType::None || Amount <= 0) continue;
		ItemCounts.FindOrAdd(Type) += Amount;
	}

	RecomputeAllFromItemCounts();
}

void UCraftInventoryComponent::RecomputeAllFromItemCounts()
{
	InitializeRecipeStates();
	RebuildCraftableCachesAndBroadcastIfChanged();
}

// ==================== Recipes ====================

void UCraftInventoryComponent::LoadRecipesFromTable()
{
	Recipes.Reset();

	if (!RecipeTable) return;

	TArray<FCraftingRecipeRow*> Rows;
	RecipeTable->GetAllRows(TEXT("CraftRecipes"), Rows);

	for (auto* Row : Rows)
	{
		if (Row) Recipes.Add(*Row);
	}

	RecipeStates.SetNum(Recipes.Num());
}

void UCraftInventoryComponent::BuildRecipeIndex()
{
	ItemToRecipes.Reset();

	for (int32 i = 0; i < Recipes.Num(); ++i)
	{
		for (const FRecipeIngredient& Req : Recipes[i].Required)
		{
			ItemToRecipes.FindOrAdd(Req.ItemType).Add(i);
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
			if (Recipes[i].Category == ERecipeCategory::Crafting)
				CraftingSet.Add(Recipes[i].ResultItemType);
			else
				CookingSet.Add(Recipes[i].ResultItemType);
		}
	}
}

// ==================== Craftable 계산 ====================

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
		Times = FMath::Min(Times, GetHave(Req.ItemType) / Req.Count);
	}

	return (Times == INT32_MAX) ? 0 : Times;
}

// ==================== UI 갱신 ====================

void UCraftInventoryComponent::RebuildCraftableCachesAndBroadcastIfChanged()
{
	CraftableTimesCrafting.Reset();
	CraftableTimesCooking.Reset();

	for (int32 i = 0; i < Recipes.Num(); ++i)
	{
		if (!RecipeStates[i].bCraftable) continue;

		const FCraftingRecipeRow& R = Recipes[i];
		const int32 Times = CalcCraftableTimesForRecipe(R);

		TMap<EItemType, int32>& MapRef =
			(R.Category == ERecipeCategory::Crafting)
			? CraftableTimesCrafting
			: CraftableTimesCooking;

		int32& Cur = MapRef.FindOrAdd(R.ResultItemType);
		Cur = FMath::Max(Cur, Times);
	}

	const bool bCountChanged =
		!AreMapsEqual(CraftableTimesCrafting, LastCraftableTimesCrafting) ||
		!AreMapsEqual(CraftableTimesCooking, LastCraftableTimesCooking);

	if (!bCountChanged) return;

	LastCraftableTimesCrafting = CraftableTimesCrafting;
	LastCraftableTimesCooking = CraftableTimesCooking;

	for (const auto& KVP : CraftableTimesCrafting)
	{
		SetCraftableItemCount(KVP.Key, KVP.Value);
	}
	for (const auto& KVP : CraftableTimesCooking)
	{
		SetCraftableItemCount(KVP.Key, KVP.Value);
	}

	RemoveNonCraftablesFromUI();
}

void UCraftInventoryComponent::SetCraftableItemCount(EItemType Type, int32 NewCount)
{
	if (NewCount < 0) NewCount = 0;

	if (UInventoryItem* Item = GetItem(Type))
	{
		Item->SetAmount(NewCount);
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Chraracter.OnUpdateCraftItem.Broadcast(Type);
		}
		return;
	}

	if (!DataList) return;

	auto* DataPtr = DataList->ItemAssetData.Find(Type);
	if (!DataPtr || !DataPtr->Get()) return;

	UInventoryItem* NewItem = NewObject<UInventoryItem>(this);
	NewItem->Initialize(DataPtr->Get());
	NewItem->SetAmount(NewCount);

	CraftableItems.Add(NewItem);

	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Chraracter.OnAddItemToCraftInventoryUI.Broadcast(NewItem);
	}
}

void UCraftInventoryComponent::RemoveNonCraftablesFromUI()
{
	TSet<EItemType> ValidTypes;
	for (const auto& KVP : CraftableTimesCrafting) if (KVP.Value > 0) ValidTypes.Add(KVP.Key);
	for (const auto& KVP : CraftableTimesCooking)  if (KVP.Value > 0) ValidTypes.Add(KVP.Key);

	for (int32 i = CraftableItems.Num() - 1; i >= 0; --i)
	{
		UInventoryItem* Item = CraftableItems[i];
		if (!Item || !ValidTypes.Contains(Item->GetData()->ItemType))
		{
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Chraracter.OnRemoveCraftItem.Broadcast(Item);
			}
			CraftableItems.RemoveAtSwap(i);
		}
	}
}

// ==================== Utils ====================

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

UInventoryItem* UCraftInventoryComponent::GetItem(EItemType Type)
{
	for (UInventoryItem* Item : CraftableItems)
	{
		if (Item && Item->GetData()->ItemType == Type)
			return Item;
	}
	return nullptr;
}
