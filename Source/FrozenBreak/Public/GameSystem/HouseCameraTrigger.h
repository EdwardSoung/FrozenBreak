// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HouseCameraTrigger.generated.h"

class UBoxComponent;

UCLASS()
class FROZENBREAK_API AHouseCameraTrigger : public AActor
{
	GENERATED_BODY()
	
public:
	AHouseCameraTrigger();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box = nullptr;

	UFUNCTION()
	void OnBoxBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
