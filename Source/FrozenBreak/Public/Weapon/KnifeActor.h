// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KnifeActor.generated.h"

UCLASS()
class FROZENBREAK_API AKnifeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKnifeActor();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Knife")
	UStaticMeshComponent* KnifeMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Knife")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Knife")
	float Range;

	UPROPERTY(EditDefaultsOnly, Category = "Knife")
	float Radius;

public:
	void PerformHitCheck(class AActionCharacter* OwnerCharacter); // 히트체크

};
