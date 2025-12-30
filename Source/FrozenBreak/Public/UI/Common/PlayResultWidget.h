// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UPlayResultWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ResultText = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> RecordText = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> LobbyButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ExitButton = nullptr;

protected:
	UFUNCTION()
	void OnClickLobby();

	UFUNCTION()
	void OnClickExit();

public:

	void UpdateRecord(int32 InDay, int32 InHour, int32 InMin);

	UPlayResultWidget* SetResult(bool IsClear);
};
