// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/WorldProp.h"

// Sets default values
AWorldProp::AWorldProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat Component"));
}

// Called when the game starts or when spawned
void AWorldProp::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWorldProp::DoAction_Implementation()
{
	//UI ¶ç¿öÁÖ±â
	//
}
