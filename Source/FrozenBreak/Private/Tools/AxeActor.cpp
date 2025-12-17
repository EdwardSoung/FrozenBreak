// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/AxeActor.h"

// Sets default values
AAxeActor::AAxeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ToolsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ToolsMesh"));
	RootComponent = ToolsMesh;

}

// Called when the game starts or when spawned
void AAxeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAxeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

