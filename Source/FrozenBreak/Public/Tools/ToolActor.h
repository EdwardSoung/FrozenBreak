// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Enums.h"
#include "ToolActor.generated.h"

UCLASS()
class FROZENBREAK_API AToolActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToolActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool") // 이 도구가 무엇인지
	EItemType ToolType = EItemType::None;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tool")
	USkeletalMeshComponent* ToolsMesh;

};
