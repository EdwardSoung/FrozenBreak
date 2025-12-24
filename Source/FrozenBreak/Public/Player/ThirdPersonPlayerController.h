// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <InputActionValue.h>
#include "ThirdPersonPlayerController.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS()
class FROZENBREAK_API AThirdPersonPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

	void OnPauseTriggered(const FInputActionValue& Value);
	void OnInventoryTriggered(const FInputActionValue& Value);
	void OnPlayerStatusTriggered(const FInputActionValue& Value);
	void OnOneStarted(const FInputActionValue& Value);
	void OnTwoStarted(const FInputActionValue& Value);
	void OnThreeStarted(const FInputActionValue& Value);
	void OnFourStarted(const FInputActionValue& Value);
	void OnFiveStarted(const FInputActionValue& Value);

    void QuickSlotExecute(int32 InSlotKey);
public:
	virtual void SetupInputComponent() override;
	UFUNCTION()
	void ToggleQuitMenu();

protected:
    // [설정] Pause 입력 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Pause;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Inventory;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_PlayerStatus;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|QuickSlot")
    TObjectPtr<UInputAction> IA_QuickSlot1;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|QuickSlot")
    TObjectPtr<UInputAction> IA_QuickSlot2;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|QuickSlot")
    TObjectPtr<UInputAction> IA_QuickSlot3;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|QuickSlot")
    TObjectPtr<UInputAction> IA_QuickSlot4;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|QuickSlot")
    TObjectPtr<UInputAction> IA_QuickSlot5;

private:
	bool bIsMenuOpen = false;
};
