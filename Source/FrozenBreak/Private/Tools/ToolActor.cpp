// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/ToolActor.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AToolActor::AToolActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	PrimaryActorTick.bCanEverTick = true;
	ToolsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ToolsMesh"));
	SetRootComponent(ToolsMesh);
}

