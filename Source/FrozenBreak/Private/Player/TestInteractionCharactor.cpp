// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TestInteractionCharactor.h"

// Sets default values
ATestInteractionCharactor::ATestInteractionCharactor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestInteractionCharactor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestInteractionCharactor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestInteractionCharactor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

