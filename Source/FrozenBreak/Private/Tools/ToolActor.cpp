// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/ToolActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "CommonComponents/StatComponent.h"
#include "Data/ItemData.h"

// Sets default values
AToolActor::AToolActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	PrimaryActorTick.bCanEverTick = true;
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatusComponent"));
	ToolsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ToolMesh"));
	SetRootComponent(ToolsMesh);
}

void AToolActor::InitializeData(UItemData* InData, float InDurability)
{
	if (StatComponent)
	{
		ToolAtkPower = 0.f;
		if (const float* AttackStats = InData->Stats.Find(EItemStatType::Attack))
		{
			ToolAtkPower = *AttackStats;
		}

		StatComponent->InitStat(InDurability, InData->Durability, ToolAtkPower);
	}
}

