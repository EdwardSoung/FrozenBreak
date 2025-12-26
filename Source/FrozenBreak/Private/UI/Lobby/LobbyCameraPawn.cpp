// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LobbyCameraPawn.h"
#include "Camera/CameraComponent.h"

// Sets default values
ALobbyCameraPawn::ALobbyCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Lobby Camera"));
	SetRootComponent(Camera);
}

// Called when the game starts or when spawned
void ALobbyCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

