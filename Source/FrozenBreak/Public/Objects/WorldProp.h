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
	virtual void DoAction_Implementation(/*���� Ÿ���� �Է¹޾Ƶ� ���� �� ����...*/) override;
	//���� : ��������(����� ����), ȹ������, �۾�����..

	virtual void OnSelect_Implementation(bool bIsStarted) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Data")
	TObjectPtr<class UStatComponent> StatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Data")
	TObjectPtr<class UPropData> Data;
};
