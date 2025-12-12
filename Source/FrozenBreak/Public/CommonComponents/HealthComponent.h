// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROZENBREAK_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	// 체력 증감 함수
	UFUNCTION(BlueprintCallable, Category = "Status|Health")
	void SetPlayerHealth(float InHealthValue);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 게임 시작 시 각 Current 값들을 Max 값으로 초기화
	virtual void InitStatus();

public:	

protected:
	// ------------------------------------------------------------------------------ Default 변수
	// 각 수치의 최소값
	float DefaultMinimum = 0.0f;
	// ------------------------------------------------------------------------------|	


	// ------------------------------------------------------------------------------ 플레이어 체력 값
	// 플레이어 최대 체력 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Health")
	float MaxHealth = 100.0f;

	// 플레이어 현재 체력 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Health")
	float CurrentHealth = 0.0f;
	// ------------------------------------------------------------------------------|

private:

};
