#include "PlayerComponents/CraftInventoryComponent.h"
#include "Objects/InventoryItem.h"
#include "GameSystem/EventSubSystem.h"
#include "GameSystem/ItemFactorySubSystem.h"
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
		EventSystem->Character.OnSendInventoryData.AddDynamic(this, &UCraftInventoryComponent::OnReceiveInventoryData);
		EventSystem->Character.OnCraftRequested.AddDynamic(this, &UCraftInventoryComponent::StartCrafting);
		EventSystem->Character.OnCookRequested.AddDynamic(this, &UCraftInventoryComponent::StartCooking);
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
			EventSystem->Character.OnUpdateCraftItem.Broadcast(Type);
		}
		return;
	}

	if (!DataList) return;

	if (UItemFactorySubSystem* ItemFactory = UItemFactorySubSystem::Get(this))
	{
		UInventoryItem* NewItem = ItemFactory->Spawn(Type);

		CraftableItems.Add(NewItem);

		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnAddItemToCraftInventoryUI.Broadcast(NewItem);
		}
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
				EventSystem->Character.OnRemoveCraftItem.Broadcast(Item);
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
	if (!CraftableItems.IsEmpty())
	{
		for (UInventoryItem* Item : CraftableItems)
		{
			if (Item && Item->GetData())
			{
				if (Item->GetData()->ItemType == Type) return Item;
			}
		}
	}
	return nullptr;
}

void UCraftInventoryComponent::StartCrafting(UInventoryItem* ItemToCraft)
{	
	if (!ItemToCraft || !ItemToCraft->GetData()) return;
	MAxCraftCost = ItemToCraft->GetData()->CraftCost;

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

void UCraftInventoryComponent::StartCooking(UInventoryItem* InIngredient)
{
	if (!InIngredient || !InIngredient->GetData() || InIngredient->GetAmount() <= 0 ) return;

	const EItemType Type = InIngredient->GetData()->ItemType;
	const int32 Amount = InIngredient->GetAmount();
	if (Type == EItemType::None || Amount <= 0) return;

	// 요리 입력 재료 누적
	CookingInputCounts.FindOrAdd(Type) += Amount;

	// 첫 번째 가능한 요리 레시피가 생겼는지 체크
	const FCraftingRecipeRow* FirstRecipe = FindFirstCookableRecipeFromInputs();
	if (!FirstRecipe)
	{
		return;
	}

	// 결과 아이템을 만들어서 StartCrafting으로 넘긴다
	if (UInventoryItem* ResultItem = GetOrCreateCraftableItem(FirstRecipe->ResultItemType))
	{
		//InIngredient->AddAmount(-1);
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnGetPickupItem.Broadcast(InIngredient->GetData()->ItemType, -1, 0);
		}
		StartCrafting(ResultItem);
	}
}

int32 UCraftInventoryComponent::GetHaveCookingInput(EItemType Type) const
{
	if (const int32* Found = CookingInputCounts.Find(Type))
		return *Found;
	return 0;
}

const FCraftingRecipeRow* UCraftInventoryComponent::FindFirstCookableRecipeFromInputs() const
{
	for (const FCraftingRecipeRow& SingleRecipe : Recipes)
	{
		if (SingleRecipe.Category != ERecipeCategory::Cooking) continue;
		if (SingleRecipe.ResultItemType == EItemType::None) continue;

		int32 Times = INT32_MAX;

		for (const FRecipeIngredient& Req : SingleRecipe.Required)
		{
			if (Req.Count <= 0) continue;
			const int32 Have = GetHaveCookingInput(Req.ItemType);
			Times = FMath::Min(Times, Have / Req.Count);
		}

		if (Times == INT32_MAX) Times = 0;

		if (Times > 0)
		{
			return &SingleRecipe; // 첫 번째 매칭 레시피
		}
	}
	return nullptr;
}

UInventoryItem* UCraftInventoryComponent::GetOrCreateCraftableItem(EItemType ResultType)
{
	if (UInventoryItem* Existing = GetItem(ResultType))
		return Existing;

	if (!DataList) return nullptr;


	if (UItemFactorySubSystem* ItemFactory = UItemFactorySubSystem::Get(this))
	{
		UInventoryItem* NewItem = ItemFactory->Spawn(ResultType);

		CraftableItems.Add(NewItem);

		return NewItem;
	}

	return nullptr;
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

	if (CurrentItemToCraft.IsValid() && CurrentItemToCraft->GetData())
	{
		const EItemType ResultType = CurrentItemToCraft->GetData()->ItemType;
		const FCraftingRecipeRow* Recipe =
			FindBestRecipeForResult(ResultType, ERecipeCategory::Crafting);

		if (Recipe)
		{
			for (const FRecipeIngredient& Req : Recipe->Required)
			{
				EventSystem->Character.OnGetPickupItem.Broadcast(
					Req.ItemType, -Req.Count, 0
				);
			}
		}


		// 초기화
		CurrentCraftCost = 0.0f;
		CurrentItemToCraft = nullptr;

		EventSystem->Character.OnGetPickupItem.Broadcast(ResultType, CraftResultCount, 0);
		EventSystem->Status.OnCurrentCraftCostChanged.Broadcast(CurrentCraftCost);

		if (UUISubSystem* UISystem = UUISubSystem::Get(this))
		{
			UISystem->HideWidget(EWidgetType::CraftProcessBar);
		}
	}
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
