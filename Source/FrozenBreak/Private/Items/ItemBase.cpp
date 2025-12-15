// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase.h"
#include "Components/WidgetComponent.h"
#include "Player/Components/InteractionComponent.h"

// Sets default values
AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// 트레이스가 맞도록 쿼리 활성화
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 인터랙션 트레이스 채널 Block (InteractionComponent와 동일 채널)
	Mesh->SetCollisionResponseToChannel(InteractableActorChannel, ECR_Block);

	PopupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TextWidget"));
	PopupWidget->SetupAttachment(RootComponent);
	PopupWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PopupWidget->SetVisibility(false);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemBase::OnInteractionStarted_Implementation()
{
	PopupWidget->SetVisibility(true);
}

void AItemBase::OnInteractionEnded_Implementation()
{
	PopupWidget->SetVisibility(false);
}

void AItemBase::OnPickup_Implementation(AActor* InstigatorActor)
{
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetLifeSpan(0.001f);
}

