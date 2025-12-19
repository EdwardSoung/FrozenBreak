// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuitMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API UQuitMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnResumeClicked();
	UFUNCTION()
	void OnQuitClicked();
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ResumeButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RecipeButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> QuitButton = nullptr;
};
