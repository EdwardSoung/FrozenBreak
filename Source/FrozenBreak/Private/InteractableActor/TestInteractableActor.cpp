// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor/TestInteractableActor.h"
#include "Components/WidgetComponent.h"
#include "Player/Components/InteractionComponent.h"

// Sets default values
ATestInteractableActor::ATestInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//SetRootComponent(Mesh);
	//Mesh->SetCollisionResponseToChannel(InteractableChannel, ECR_Block);
	//
	//PopupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TextWidget"));
	//PopupWidget->SetupAttachment(RootComponent);
	//PopupWidget->SetRelativeLocation(FVector(0, 0, 100.f));
	//PopupWidget->SetWidgetSpace(EWidgetSpace::Screen);
	//PopupWidget->SetVisibility(false);
}

// Called when the game starts or when spawned
void ATestInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

//void ATestInteractableActor::OnInteractionStarted_Implementation()
//{
//	//PopupWidget->SetVisibility(true);
//}
//
//void ATestInteractableActor::OnInteractionEnded_Implementation()
//{
//	//PopupWidget->SetVisibility(false);
//}
//
//void ATestInteractableActor::OnPickup_Implementation()
//{
//	//SetActorHiddenInGame(true);
//	//SetActorTickEnabled(false);
//	//SetLifeSpan(0.001f);
//}

