// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "GameSystem/Events/StatusEvents.h"
#include "WorldProp.generated.h"

UCLASS()
class FROZENBREAK_API AWorldProp : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldProp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:		
	UFUNCTION(BlueprintCallable)
	virtual void DoAction_Implementation(/*동작 타입을 입력받아도 좋을 것 같음...*/) override;
	//예시 : 공격인지(벌목등 포함), 획득인지, 작업인지..
	UFUNCTION(BlueprintCallable)
	virtual void OnSelect_Implementation(bool bIsStarted) override;

	UFUNCTION(BlueprintPure, Category = "Prop|Data")
	EItemType GetInteractableToolType() const;
	
	UFUNCTION(BlueprintPure, Category = "Prop|Data")
	EPropType GetPropType() const;

	virtual void CampfireAction() {};

	inline float GetWorkFatigue() { return FatigueCostPerWork; }
private:
	const ECollisionChannel InteractableActorChannel = ECollisionChannel::ECC_GameTraceChannel1;
	const float BedUsageHours = 4.0f;

private:
	// 플레이어가 나무와 상호작용 시
	void TreeAction();

	// 플레이어가 바위와 상호작용 시
	void RockAction();

	// 플레이어가 침대와 상호작용 시
	void BedAction();

	// 지금이 잘 수 있는 시간인지 알려줌
	void IsBedTime();

	// 내구도 비율 계산해주는 함수
	float GetDurabilityRadio() const;

	// BedActionWidget이 시작 됐을 때
	UFUNCTION()
	void BedActionWidgetStarted();

	// BedActionWidget 애니메이션의 중간 지점일 때
	UFUNCTION()
	void BedActionWidgetMid();

	// BedActionWidget 애니메이션이 끝났을 때
	UFUNCTION()
	void BedActionWidgetFinished();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Stat")
	TObjectPtr<class UStatComponent> StatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Data")
	TObjectPtr<class UPropData> Data;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop|Mesh")
	TObjectPtr<class UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop|Widget")
	TObjectPtr<class UWidgetComponent> InteractionWidget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop|Widget")
	TObjectPtr<class UWidgetComponent> DurabilityWidget = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APickupItem> GenerateItemClass = nullptr;

	UPROPERTY()
	class UEventSubSystem* EventSystem = nullptr;

	// BedAction 위젯 WBP 클래스 (에디터에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UBedActionWidget> BedActionWidgetClass = nullptr;

	// 현재 표시 중인 BedAction 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<class UBedActionWidget> BedActionWidgetInstance = nullptr;

protected:
	// 작업시 소모시킬 피로도 (최종(확정된) 값이 아님)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Data")
	float FatigueCostPerWork = -0.5f;

	// 침대 사용시 감소시킬 배고픔
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Data")
	float HungerReductionAmount = -30.f;

	bool bIsBedTime = false;

	float ToolAtkPower = 0.f;

	float CurrentDurability = 0;

	float MaxDurability = 0;

	// Rock 용 StoneSpawnCount. Stone 을 몇번 생성 시켰냐
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop|Data")
	int32 CurrentSpawnCount = 0;
	// Stone이 MaxSpawnCount가 되면 Rock은 파괴 됨.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop|Data")
	int32 MaxSpawnCount = 10;

	// BedActionWidget에 보낼 n일차 텍스트
	int32 DayCount = 0;
};
