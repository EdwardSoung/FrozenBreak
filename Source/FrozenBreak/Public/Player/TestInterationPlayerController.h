// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "TestInterationPlayerController.generated.h"

class UInputMappingContext;	// UInputMappingContext라는 클래스가 있다고 알려주는 역할
class UInputAction;

/**
 * 
 */
UCLASS()
class FROZENBREAK_API ATestInterationPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void SetupInputComponent() override;

private:
	void OnCameraInput(const FInputActionValue& InValue);


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Camera = nullptr;

private:
	int32 GameInputPriority = 1;
};
