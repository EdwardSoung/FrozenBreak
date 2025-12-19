// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/KnifeActor.h"
#include "Player/ActionCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKnifeActor::AKnifeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    KnifeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KnifeMesh"));
    RootComponent = KnifeMesh;

    Damage = 20.0f;
    Range = 160.0f;
    Radius = 70.0f;

}

// Called when the game starts or when spawned


void AKnifeActor::PerformHitCheck(AActionCharacter* OwnerCharacter)
{
    if (!OwnerCharacter) return;

    FVector Start = OwnerCharacter->GetActorLocation() + FVector(0, 0, 60);
    FVector End = Start + OwnerCharacter->GetActorForwardVector() * Range;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerCharacter);

    TArray<FHitResult> Hits;

    bool bHit = GetWorld()->SweepMultiByChannel(
        Hits,
        Start,
        End,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(Radius),
        Params
    );

    if (!bHit) return;

    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor) continue;

        UGameplayStatics::ApplyDamage(
            HitActor,
            Damage,
            OwnerCharacter->GetController(),
            OwnerCharacter,
            nullptr
        );
    }
}
