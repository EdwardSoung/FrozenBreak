// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ItemData.h"
#include "PlayerStatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROZENBREAK_API UPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerStatComponent();
	//온도 수치
	UFUNCTION(BlueprintCallable, Category = "Status|Temperature")
	void SetPlayerTemperature(float InTemperatureValue);

	//피로도 수치
	UFUNCTION(BlueprintCallable, Category = "Status|Stamina")
	void SetPlayerFatigue(float InFatigueValue);

	//배고픔 수치
	UFUNCTION(BlueprintCallable, Category = "Status|Hunger")
	void SetPlayerHunger(float InHungerValue);

	float GetCurrentFatigue() { return CurrentFatigue; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void InitStatus();
	void BindStatSettingEvents();

	UFUNCTION()
	void ItemUsed(UInventoryItem* InItem);

	UFUNCTION()
	void EquipHandItemUsed();

	UFUNCTION()
	void RefreshUI();

	UFUNCTION()
	void SendCurrentFatigue();

public:

protected:
	// ------------------------------------------------------------------------------ 플레이어 온도 값
	// 최대 온도 수치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status|Temperature")
	float MaxTemperature = 100.0f;

	// 현재 온도 수치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Temperature")
	float CurrentTemperature = 0.0f;
	// ------------------------------------------------------------------------------|


	// ------------------------------------------------------------------------------ 플레이어 활동력 값
	// 최대 활동력 수치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status|Fatigue")
	float MaxFatigue = 100.0f;

	// 현재 활동력 수치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Fatigue")
	float CurrentFatigue = 0.0f;
	// ------------------------------------------------------------------------------|


	// ------------------------------------------------------------------------------ 플레이어 포만감 값
	// 최대 포만감 수치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status|Hunger")
	float MaxHunger = 100.0f;

	// 현재 포만감 수치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Hunger")
	float CurrentHunger = 0.0f;
	// ------------------------------------------------------------------------------|

	// Data Asset 형태의 상태 지속 증감값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	TObjectPtr<class UStatusDataAsset> StatusDataAsset = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<class UInventoryItem> HandEquip;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<class UInventoryItem> BodyEquip;
};
