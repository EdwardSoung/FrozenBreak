// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/HouseCameraTrigger.h"
#include "Components/BoxComponent.h"
#include "Player/ActionCharacter.h"


// Sets default values
AHouseCameraTrigger::AHouseCameraTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);

	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Box->OnComponentBeginOverlap.AddDynamic(this, &AHouseCameraTrigger::OnBoxBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &AHouseCameraTrigger::OnBoxEnd);
}

void AHouseCameraTrigger::OnBoxBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AActionCharacter* Char = Cast<AActionCharacter>(OtherActor))
	{
		Char->SetIndoorCameraZoom(true);
	}

}

void AHouseCameraTrigger::OnBoxEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AActionCharacter* Char = Cast<AActionCharacter>(OtherActor))
	{
		Char->SetIndoorCameraZoom(false);
	}
}

