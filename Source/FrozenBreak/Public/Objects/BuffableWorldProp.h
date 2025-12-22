// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/WorldProp.h"
#include "BuffableWorldProp.generated.h"

/**
 * 
 */
UCLASS()
class FROZENBREAK_API ABuffableWorldProp : public AWorldProp
{
	GENERATED_BODY()
	
public:
	ABuffableWorldProp();

	virtual void OnSelect_Implementation(bool bIsStarted) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginBuff(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void EndBuff(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	virtual void CampfireAction() override;

	void IsCookValid();

private:
	void GiveCampFireBuff();
	void ReadyToCook();
	UFUNCTION()
	void SetMeets(TArray<UInventoryItem*> InData);
	void FinishCamFireBuff();

	void StartCampfireTimer();

	UFUNCTION()
	void CampfireTick();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USphereComponent> BuffArea = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Stat")
	float BuffAreaRadius = 200.0f;

	// Prop이 가지는 각 스탯 값 버프량은 데이터 애셋으로 관리
	// 추후 또다른 Prop이 추가될 경우 해당하는 데이터 애셋 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Stat")
	TObjectPtr<class UStatBuffValueData> CampfireBuffValues = nullptr;

	UPROPERTY()
	TObjectPtr<class UPropDurabilityWidget> CampfireLifeBar = nullptr;
private:
	class UStatusCalculationSubSystem* StatusCalculater;
	EPropType PropType = EPropType::None;

	TArray<UInventoryItem*> Meets;

	// 모닥불 수명 관리
	FTimerHandle CampfireHandle;
	float CampfireTimerRate = 0.1f;
};
