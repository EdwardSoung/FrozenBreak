// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Data/Enums.h"
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
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<class UDataTable> WidgetData;
};
