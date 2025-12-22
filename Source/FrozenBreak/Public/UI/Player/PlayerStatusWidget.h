// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/UI/Openable.h"
#include "PlayerStatusWidget.generated.h"

class UItemData;
class UInventoryItem;
/**
 * 
 */
UCLASS()
class FROZENBREAK_API UPlayerStatusWidget : public UUserWidget, public IOpenable
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void SetInGameTime(FText InTimeValue);

	UFUNCTION()
	void SetTemperatureDefence(FText InDefenceValue);

	UFUNCTION()
	void SetHandEquipment(UInventoryItem* InHandItem);

	UFUNCTION()
	void SetBodyEquipment(UInventoryItem* InBodyItem);
	UFUNCTION()
	void HideWidget();

public:
	void OpenWidget_Implementation();
	void CloseWidget_Implementation();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEquipmentSlotWidget> HandEquipment = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEquipmentSlotWidget> BodyEquipment = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> InGameTime = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TemperatureDefence = nullptr;

	UPROPERTY(meta = (BindWidget))	
	TObjectPtr<class UButton> CloseStatButton = nullptr;
private:
	// 초기화용 기본 아이콘 : null
	UTexture2D* DefaultItemIcon = nullptr;
	
	// 초기화용 기본 장착 아이템명
	FText DefaultItemName = FText::FromString(TEXT("장비"));

	// 초기화용 기본 시간 값
	FText DefaultGameTime = FText::FromString(TEXT("00:00"));
	
	// 초기화용 기본 방한 값
	FText DefaultTemperatureDefence = FText::FromString(TEXT("0.0"));
};
