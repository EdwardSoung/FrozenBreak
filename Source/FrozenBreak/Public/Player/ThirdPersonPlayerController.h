// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <InputActionValue.h>
#include "ThirdPersonPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API AThirdPersonPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	void OnPauseTriggered(const FInputActionValue& Value);

public:
	virtual void SetupInputComponent() override;

protected:
    // [설정] Pause 입력 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<class UInputAction> IA_Pause;

private:
	bool bIsMenuOpen = false;
};
