// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Data/Enums.h"
#include "Data/ItemDataList.h"
#include "Data/EquipItemData.h"
#include "GameSystem/EventSubSystem.h"
#include "GameManager.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UGameManager : public UGameInstance
{
	GENERATED_BODY()
	
public:
	static UGameManager* Get(const UObject* WorldContextObject);

	//Data Table / Asset 등의 자료, Blueprint 오브젝트 직접 연결 시 GameInstance에서 사용합니다.
	TSubclassOf<class UUserWidget> GetWidgetClass(EWidgetType InType);

	TSubclassOf<class APickupItem> GetPickupItemClass(EItemType InType);

	UItemData* GetItemData(EItemType InType);

	TSubclassOf<class AToolActor> GetToolClass(EItemType InType);

	uint32 GetUID();

	inline float GetInGameStartTime() { return InGameStartTime; }

	void SetState(EGameState InState);
	inline EGameState GetGameState() { return CurrentState; }
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<class UDataTable> WidgetData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<class UDataTable> PickupItemTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UItemDataList> ItemDataList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UEquipItemData> EquipItemData;

	// 인게임 시작 시간(0~1 : 0.25 = 6시, 0.5 = 12시, 0.75 = 18시, 1.0 = 00시)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float InGameStartTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameData")
	EGameState CurrentState;

private:
	uint32 CurrentUID = 1;
};
