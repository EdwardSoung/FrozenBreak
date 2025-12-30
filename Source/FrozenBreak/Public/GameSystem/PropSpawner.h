// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PropSpawner.generated.h"

class UBoxComponent;

UCLASS()
class FROZENBREAK_API APropSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	APropSpawner();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBoxComponent* SpawnVolume;

    UPROPERTY(EditAnywhere, Category = "Settings")
    TArray<TSubclassOf<AActor>> ActorToSpawn;

    UPROPERTY(EditAnywhere, Category = "Settings")
    int32 SpawnCount = 10;

    //회전 주기위함
    UPROPERTY(EditAnywhere, Category = "Settings")
    bool bRandomYaw = true;

    UPROPERTY(EditAnywhere, Category = "Settings")
    float MinScaleSize = 1.0f;
    UPROPERTY(EditAnywhere, Category = "Settings")
    float MaxScaleSize = 1.0f;

public:
    // [버튼] 생성
    UFUNCTION(CallInEditor, Category = "Editor Tools")
    void SpawnActors();

    // [버튼] 지우기
    UFUNCTION(CallInEditor, Category = "Editor Tools")
    void ClearGeneratedActors();

private:
    //만든 액터들 저장(삭제용)
    UPROPERTY()
    TArray<AActor*> SpawnedActors;

};
