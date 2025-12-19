#include "PlayerComponents/CraftInventoryComponent.h"
#include "Objects/InventoryItem.h"
#include "GameSystem/EventSubSystem.h"
#include "Data/ItemDataList.h"
#include <GameSystem/UISubSystem.h>

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
		EventSystem->Chraracter.OnSendInventoryData.AddDynamic(this, &UCraftInventoryComponent::OnReceiveInventoryData);
		EventSystem->Chraracter.OnCraftRequested.AddDynamic(this, &UCraftInventoryComponent::StartCrafting);
;	}
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

void UCraftInventoryComponent::StartCrafting(UInventoryItem* ItemToCraft)
{
	UE_LOG(LogTemp, Log, TEXT("StartCrafting : %d"), ItemToCraft->GetData()->ItemType);
	
	MAxCraftCost = ItemToCraft->GetData()->CraftCost;
	UE_LOG(LogTemp, Log, TEXT("StartCrafting cost : %.2f"), MAxCraftCost);

	if (MAxCraftCost > 0)
	{
		CurrentItemToCraft = ItemToCraft;
		GetWorld()->GetTimerManager().ClearTimer(CraftHandle);

		if (UUISubSystem* UISystem = UUISubSystem::Get(this))
		{
			UISystem->ShowWidget(EWidgetType::CraftProcessBar);
		}

		GetWorld()->GetTimerManager().SetTimer(
			CraftHandle,
			this,
			&UCraftInventoryComponent::SetCraftProcess,
			CraftRate,
			true
		);
	}
}

void UCraftInventoryComponent::SetCraftProcess()
{
	UEventSubSystem* EventSystem = UEventSubSystem::Get(this);
	if (!EventSystem) return;

	if (MAxCraftCost <= 0.f) return;

	const float DeltaCost = CraftSpeedPerSecond * CraftRate;

	CurrentCraftCost = FMath::Clamp(
		CurrentCraftCost + DeltaCost,
		0.0f,
		MAxCraftCost
	);

	// UI 갱신 (0~1)
	EventSystem->Status.OnCurrentCraftCostChanged.Broadcast(
		CurrentCraftCost / MAxCraftCost
	);

	UE_LOG(LogTemp, Verbose, TEXT("Crafting... %.2f / %.2f"),
		CurrentCraftCost, MAxCraftCost);

	// 완료 체크
	if (CurrentCraftCost >= MAxCraftCost)
	{
		FinishCraft();
	}
}

void UCraftInventoryComponent::FinishCraft()
{
	UEventSubSystem* EventSystem = UEventSubSystem::Get(this);
	if (!EventSystem) return;

	UE_LOG(LogTemp, Log, TEXT("제작 완료!"));

	const EItemType ResultType = CurrentItemToCraft->GetData()->ItemType;
	const FCraftingRecipeRow* Recipe =
		FindBestRecipeForResult(ResultType, ERecipeCategory::Crafting);

	if (Recipe)
	{
		for (const FRecipeIngredient& Req : Recipe->Required)
		{
			EventSystem->Chraracter.OnGetPickupItem.Broadcast(
				Req.ItemType, -Req.Count
			);
		}
	}

	EventSystem->Chraracter.OnGetPickupItem.Broadcast(
		ResultType, CraftResultCount
	);

	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->HideWidget(EWidgetType::CraftProcessBar);
	}

	// 초기화
	CurrentCraftCost = 0.f;
	CurrentItemToCraft = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(CraftHandle);
}

bool UCraftInventoryComponent::CanConsumeRecipeOnce(const FCraftingRecipeRow& R) const
{
	for (const FRecipeIngredient& Req : R.Required)
	{
		if (GetHave(Req.ItemType) < Req.Count)
			return false;
	}
	return true;
}

// 현재 인벤에서 제작 가능 횟수가 가장 큰 레시피를 선택
const FCraftingRecipeRow* UCraftInventoryComponent::FindBestRecipeForResult(EItemType ResultType, ERecipeCategory Category) const
{
	const FCraftingRecipeRow* Best = nullptr;
	int32 BestTimes = -1;

	for (const FCraftingRecipeRow& R : Recipes)
	{
		if (R.Category != Category) continue;
		if (R.ResultItemType != ResultType) continue;

		// 이 레시피 기준 가능 횟수
		int32 Times = INT32_MAX;
		for (const FRecipeIngredient& Req : R.Required)
		{
			if (Req.Count <= 0) continue;
			const int32 Have = GetHave(Req.ItemType);
			Times = FMath::Min(Times, Have / Req.Count);
		}
		if (Times == INT32_MAX) Times = 0;

		if (Times > BestTimes)
		{
			BestTimes = Times;
			Best = &R;
		}
	}
	return Best;
}

void UCraftInventoryComponent::ConsumeRecipeOnce(const FCraftingRecipeRow& R)
{
	for (const FRecipeIngredient& Req : R.Required)
	{
		int32& Count = ItemCounts.FindOrAdd(Req.ItemType);
		Count = FMath::Max(0, Count - Req.Count);
	}
}
