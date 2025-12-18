// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonComponents/StatComponent.h"
#include "CraftComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FROZENBREAK_API UCraftComponent : public UStatComponent
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void BeginPlay() override;
	void BindEvents();

	UFUNCTION()
	void SstCurrentCraftCost(float InCraftAmount);

	UFUNCTION(BlueprintCallable)
	void TestCraft(float InCraftAmount);

private:
	UFUNCTION()
	void StartCraft(float InCraftAmount);

protected:
	// 제작에 필요한 총 작업량(체력과 비슷한 개념)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxCraftCost = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentCraftCost = 0.0f;

	// 제작에 사용되는 작업력(공격력과 비슷한 개념)
	// 예) 100의 작업량을 가진 아이템을 초당 10의 작업력으로 작업 시 10초가 걸린다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CraftAmount = 10.0f;

private:
	// 제작 시작 시 가지는 현재 작업량(기본적으로 0)
	float BaseCraftCost = 0.0f;

	// 제작 타이머 관련 변수
	FTimerHandle CraftHandle;
	float CraftRate = 1.0f;
};
