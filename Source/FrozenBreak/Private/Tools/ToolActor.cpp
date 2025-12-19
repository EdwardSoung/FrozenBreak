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

void AToolActor::BeginPlay()
{
	Super::BeginPlay();

	if (StatComponent)
	{
		if (Data)
		{
			// Attack을 가지지 않은 아이템의 Attack 기본값 (0)
			ToolAtkPower = 0.f;
			if (const float* AttackStats = Data->Stats.Find(EItemStatType::Attack))
			{
				ToolAtkPower = *AttackStats;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("아이템에 Attack 스텟이 설정되어 있지 않거나, 필요없는 아이템 입니다."));
			}
			// 데이터 에셋에 작성된 데이터를 StatComponent로 보낸다.
			StatComponent->InitStat(Data->Durability, ToolAtkPower);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup Item Begin : 아이템 데이터가 없음!"));
		}
	}
}

