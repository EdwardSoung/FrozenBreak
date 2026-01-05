// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Enums.h"
#include "UISubSystem.generated.h"

class UUserWidget;
/**
 * 
 */


UCLASS()
class FROZENBREAK_API UUISubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UUISubSystem* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	UUserWidget* ShowWidget(EWidgetType InWidgetType, EInputModeType InInputMode = EInputModeType::GameAndUI);

	UFUNCTION(BlueprintCallable)
	void HideWidget(EWidgetType InWidgetType);

	bool IsOpenedWidget(EWidgetType InWidgetType);
	
	void HideAllWiget();
	void ClearAllWidgets();
private:
	UPROPERTY()
	TMap<EWidgetType, UUserWidget*> CreatedWidgets;
	UPROPERTY()
	TMap<EWidgetType, UUserWidget*> OpenedWidgets;

	TWeakObjectPtr<class APlayerController> CurrentPlayerController = nullptr;
};
