// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnClickStart();
	UFUNCTION()
	void OnClickExit();
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> StartButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ExitButton;
};
