// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Enums.h"
#include "ToolActor.generated.h"

// 이제 안 씀. 나중에 청소 해야함

UCLASS()
class FROZENBREAK_API AToolActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToolActor();

	inline float GetToolAtkPower() const { return ToolAtkPower; }

	void InitializeData(class UItemData* InData, float InDurability);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool") // 이 도구가 무엇인지
	EItemType ToolType = EItemType::None;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tool")
	TObjectPtr<class USkeletalMeshComponent> ToolsMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tool|Stat")
	TObjectPtr<class UStatComponent> StatComponent = nullptr;

protected:
	float ToolAtkPower = 0;
};
