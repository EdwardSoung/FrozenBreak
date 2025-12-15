// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PickupItem.h"
#include "Data/ItemData.h"
#include "CommonComponents/StatComponent.h"

// Sets default values
APickupItem::APickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat Component"));
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();
		
	if (StatComponent)
	{
		if (Data)
		{
			StatComponent->InitStat(Data->Health, Data->Attack);
		}
	}
}

