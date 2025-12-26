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
	// Sets default values for this actor's properties
	APropSpawner();

protected:
    // 영역을 표시할 박스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBoxComponent* SpawnVolume;

    // 생성할 블루프린트 클래스
    UPROPERTY(EditAnywhere, Category = "Settings")
    TArray<TSubclassOf<AActor>> ActorToSpawn;

    // 몇 개나 뿌릴지
    UPROPERTY(EditAnywhere, Category = "Settings")
    int32 SpawnCount = 10;

    // 랜덤 회전 줄까? (제자리 뱅글뱅글)
    UPROPERTY(EditAnywhere, Category = "Settings")
    bool bRandomYaw = true;

    UPROPERTY(EditAnywhere, Category = "Settings")
    float MinScaleSize = 1.0f;
    UPROPERTY(EditAnywhere, Category = "Settings")
    float MaxScaleSize = 1.0f;

public:
    // [버튼] 에디터에서 누르면 실행됨
    UFUNCTION(CallInEditor, Category = "Editor Tools")
    void SpawnActors();

    // [버튼] 잘못 뿌렸을 때 싹 지우기 (Undo 대용)
    UFUNCTION(CallInEditor, Category = "Editor Tools")
    void ClearGeneratedActors();

    UFUNCTION(CallInEditor, Category = "Editor Tools")
    FVector GetRandomPointOnGround();
private:
    // 내가 만든 애들을 기억해두는 배열 (나중에 지우려고)
    UPROPERTY()
    TArray<AActor*> SpawnedActors;

};
