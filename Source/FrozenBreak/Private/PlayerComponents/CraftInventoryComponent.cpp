#include "PlayerComponents/CraftInventoryComponent.h"
#include "Objects/InventoryItem.h"
#include "GameSystem/EventSubSystem.h"
#include "GameSystem/ItemFactorySubSystem.h"
#include "GameSystem/UISubSystem.h"
#include "GameSystem/StatusCalculationSubSystem.h"
#include "Data/ItemDataList.h"

UCraftInventoryComponent::UCraftInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCraftInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadRecipesFromTable(); // 레시피 로드만

	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Character.OnSendInventoryData.AddDynamic(this, &UCraftInventoryComponent::OnReceiveInventoryData);
		EventSystem->Character.OnCraftRequested.AddDynamic(this, &UCraftInventoryComponent::StartCrafting);
		EventSystem->Character.OnCookRequested.AddDynamic(this, &UCraftInventoryComponent::StartCooking);

		EventSystem->Character.OnFatigueChecked.AddDynamic(this, &UCraftInventoryComponent::SetCurrentFatigue);
		EventSystem->Character.OnRequesetFatigueCheck.Broadcast();
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
	RebuildCraftableUIIfChanged();
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

void UCraftInventoryComponent::InitializeRecipeStates()
{
	CraftableResultsCrafting.Reset();
	CraftableResultsCooking.Reset();

	for (int32 i = 0; i < Recipes.Num(); ++i)
	{
		FRecipeState& S = RecipeStates[i];
		S.MissingKinds = 0;
		S.MissingCount.Reset();

		const FCraftingRecipeRow& R = Recipes[i];

		for (const FRecipeIngredient& Req : R.Required)
		{
			const int32 Have = GetHave(Req.ItemType);
			const int32 Missing = FMath::Max(Req.Count - Have, 0);

			S.MissingCount.Add(Req.ItemType, Missing);
			if (Missing > 0) S.MissingKinds++;
		}

		S.bCraftable = (S.MissingKinds == 0);

		if (S.bCraftable)
		{
			if (R.Category == ERecipeCategory::Crafting)
				CraftableResultsCrafting.Add(R.ResultItemType);
			else
				CraftableResultsCooking.Add(R.ResultItemType);
		}
	}
}

int32 UCraftInventoryComponent::GetHave(EItemType Type) const
{
	if (const int32* Found = ItemCounts.Find(Type))
		return *Found;
	return 0;
}

// ==================== UI 갱신 ====================

static bool AreSetsEqual(const TSet<EItemType>& A, const TSet<EItemType>& B)
{
	if (A.Num() != B.Num()) return false;
	for (const EItemType& T : A)
	{
		if (!B.Contains(T)) return false;
	}
	return true;
}

void UCraftInventoryComponent::RebuildCraftableUIIfChanged()
{
	// 변경 없으면 UI 갱신 스킵
	const bool bChanged =
		!AreSetsEqual(CraftableResultsCrafting, LastCraftableResultsCrafting) ||
		!AreSetsEqual(CraftableResultsCooking, LastCraftableResultsCooking);

	if (!bChanged) return;

	LastCraftableResultsCrafting = CraftableResultsCrafting;
	LastCraftableResultsCooking = CraftableResultsCooking;

	// Crafting 결과들 UI에 존재 보장
	for (const EItemType Type : CraftableResultsCrafting)
	{
		EnsureCraftableItemExists(Type);
	}

	RemoveNonCraftablesFromUI();
}

void UCraftInventoryComponent::EnsureCraftableItemExists(EItemType Type)
{
	if (Type == EItemType::None) return;
	if (GetItem(Type)) return;

	if (!DataList) return;

	if (UItemFactorySubSystem* ItemFactory = UItemFactorySubSystem::Get(this))
	{
		UInventoryItem* NewItem = ItemFactory->Spawn(Type);
		if (!NewItem) return;

		CraftableItems.Add(NewItem);

		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Character.OnAddItemToCraftInventoryUI.Broadcast(NewItem);
		}
	}
}

void UCraftInventoryComponent::RemoveNonCraftablesFromUI()
{
	TSet<EItemType> ValidTypes = CraftableResultsCrafting;
	ValidTypes.Append(CraftableResultsCooking);

	for (int32 i = CraftableItems.Num() - 1; i >= 0; --i)
	{
		UInventoryItem* Item = CraftableItems[i];
		if (!Item || !Item->GetData() || !ValidTypes.Contains(Item->GetData()->ItemType))
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

UInventoryItem* UCraftInventoryComponent::GetItem(EItemType Type)
{
	for (UInventoryItem* Item : CraftableItems)
	{
		if (Item && Item->GetData() && Item->GetData()->ItemType == Type)
			return Item;
	}
	return nullptr;
}

const FCraftingRecipeRow* UCraftInventoryComponent::FindRecipeForResult(EItemType ResultType, ERecipeCategory Category) const
{
	for (const FCraftingRecipeRow& R : Recipes)
	{
		if (R.Category == Category && R.ResultItemType == ResultType)
			return &R;
	}
	return nullptr;
}

// ==================== Craft ====================

void UCraftInventoryComponent::StartCrafting(UInventoryItem* ItemToCraft)
{
	if (!ItemToCraft || !ItemToCraft->GetData() || IsCrafting) return;

	MaxCraftCost = ItemToCraft->GetData()->CraftCost;
	if (MaxCraftCost <= 0.f) return;
	if (!HasEnoughFatigue()) return;
	

	const EItemType ResultType = ItemToCraft->GetData()->ItemType;
	

	CurrentItemToCraft = ItemToCraft;
	CurrentCraftCost = 0.0f;
	GetWorld()->GetTimerManager().ClearTimer(CraftHandle);

	if (const FCraftingRecipeRow* Recipe = FindRecipeForResult(ResultType, ERecipeCategory::Crafting))
	{
		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			for (const FRecipeIngredient& Req : Recipe->Required)
			{
				EventSystem->Character.OnGetPickupItem.Broadcast(Req.ItemType, -Req.Count, 0);
			}
		}
	}

	if (UStatusCalculationSubSystem* StatusSS = GetWorld()->GetSubsystem<UStatusCalculationSubSystem>())
	{
		StatusSS->DecreaseFatigue(FatigueUse);
	}

	if (UUISubSystem* UISystem = UUISubSystem::Get(this))
	{
		UISystem->ShowWidget(EWidgetType::CraftProcessBar);
	}

	IsCrafting = true;
	GetWorld()->GetTimerManager().SetTimer(
		CraftHandle,
		this,
		&UCraftInventoryComponent::SetCraftProcess,
		CraftRate,
		true
	);
}

void UCraftInventoryComponent::SetCraftProcess()
{
	UEventSubSystem* EventSystem = UEventSubSystem::Get(this);
	if (!EventSystem) return;
	if (MaxCraftCost <= 0.f) return;

	CurrentCraftCost = FMath::Clamp(CurrentCraftCost + CraftRate * CraftAmount, 0.0f, MaxCraftCost);

	EventSystem->Status.OnCurrentCraftCostChanged.Broadcast(CurrentCraftCost / MaxCraftCost);

	if (CurrentCraftCost >= MaxCraftCost)
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
		EventSystem->Character.OnGetPickupItem.Broadcast(CurrentItemToCraft->GetData()->ItemType, CraftResultCount, 0);

		if (UUISubSystem* UISystem = UUISubSystem::Get(this))
		{
			UISystem->HideWidget(EWidgetType::CraftProcessBar);
		}
	}

	// 초기화
	CurrentCraftCost = 0.0f;
	CurrentItemToCraft = nullptr;

	EventSystem->Status.OnCurrentCraftCostChanged.Broadcast(CurrentCraftCost);

	if (UStatusCalculationSubSystem* StatusSS = GetWorld()->GetSubsystem<UStatusCalculationSubSystem>())
	{
		StatusSS->IncreaseFatigue(FatigueUse);
	}
	if (IsCooking)
	{
		IsCooking = false;
	}
	if (IsCrafting)
	{
		IsCrafting = false;
	}

	GetWorld()->GetTimerManager().ClearTimer(CraftHandle);
	EventSystem->Character.OnRequesetFatigueCheck.Broadcast();
}

// ==================== Cooking ====================

void UCraftInventoryComponent::StartCooking(UInventoryItem* InIngredient)
{
	if (!InIngredient || !InIngredient->GetData() || InIngredient->GetAmount() <= 0) return;

	const EItemType Type = InIngredient->GetData()->ItemType;
	if (Type == EItemType::None) return;

	CookingInputCounts.FindOrAdd(Type) += 1;

	const FCraftingRecipeRow* FirstRecipe = FindFirstCookableRecipeFromInputs();
	if (!FirstRecipe) return;

	if (!IsCooking)
	{
		if (UInventoryItem* ResultItem = GetOrCreateCraftableItem(FirstRecipe->ResultItemType))
		{
			if (ResultItem->GetData()->CraftCost > CurrentFatigue) return;
			if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
			{
				EventSystem->Character.OnGetPickupItem.Broadcast(Type, -1, 0);
			}
			IsCooking = true;
			StartCrafting(ResultItem);
		}
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
	for (const FCraftingRecipeRow& R : Recipes)
	{
		if (R.Category != ERecipeCategory::Cooking) continue;
		if (R.ResultItemType == EItemType::None) continue;

		int32 Times = INT32_MAX;
		for (const FRecipeIngredient& Req : R.Required)
		{
			if (Req.Count <= 0) continue;
			Times = FMath::Min(Times, GetHaveCookingInput(Req.ItemType) / Req.Count);
		}
		if (Times == INT32_MAX) Times = 0;

		if (Times > 0) return &R;
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
		if (!NewItem) return nullptr;

		CraftableItems.Add(NewItem);
		return NewItem;
	}
	return nullptr;
}

void UCraftInventoryComponent::SetCurrentFatigue(float InValue)
{
	CurrentFatigue = InValue;
}
