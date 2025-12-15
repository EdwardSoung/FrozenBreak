// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
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
	virtual void DoAction_Implementation(/*동작 타입을 입력받아도 좋을 것 같음...*/) override;
	//예시 : 공격인지(벌목등 포함), 획득인지, 작업인지..

	virtual void OnSelect_Implementation(bool bIsStarted) override;

	void InitStat(float InAttack, float InHealth);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Data")
	TObjectPtr<class UStatComponent> StatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Data")
	TObjectPtr<class UPropData> Data;
};
