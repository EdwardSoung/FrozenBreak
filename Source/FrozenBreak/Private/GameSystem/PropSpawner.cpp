// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/PropSpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

APropSpawner::APropSpawner()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
    RootComponent = SpawnVolume;

    SpawnVolume->SetLineThickness(5.0f); //라인 잘 안보여서 변경

}


void APropSpawner::SpawnActors()
{
    if (ActorToSpawn.Num() <= 0) return;

    // 박스 가져오기
    FVector Origin = SpawnVolume->GetComponentLocation();
    FVector BoxExtent = SpawnVolume->GetScaledBoxExtent();

    for (int32 i = 0; i < SpawnCount; i++)
    {
        // 1. 박스 범위 안에서 랜덤한 X, Y 좌표 뽑기
        float RandomX = FMath::RandRange(Origin.X - BoxExtent.X, Origin.X + BoxExtent.X);
        float RandomY = FMath::RandRange(Origin.Y - BoxExtent.Y, Origin.Y + BoxExtent.Y);

        FVector TraceStart = FVector(RandomX, RandomY, Origin.Z + BoxExtent.Z);
        FVector TraceEnd = FVector(RandomX, RandomY, Origin.Z - BoxExtent.Z - 2000.f);


        FHitResult HitResult;
        FCollisionQueryParams Params;
        //자기자신은 무시
        Params.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            TraceStart,
            TraceEnd,
            ECC_GameTraceChannel2,
            Params
        );

        if (bHit)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                FString::Printf(TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName()));
            //회전값 설정
            FRotator SpawnRotation = FRotator::ZeroRotator;
            if (bRandomYaw)
            {
                SpawnRotation.Yaw = FMath::RandRange(0.0f, 360.0f);
            }
            
            //랜덤 한개 가져오기
            int32 RandomIndex = FMath::RandRange(0, ActorToSpawn.Num() - 1);
            auto SpawnActor = ActorToSpawn[RandomIndex];

            AActor* NewActor = GetWorld()->SpawnActor<AActor>(SpawnActor, HitResult.Location, SpawnRotation);

            if (NewActor)
            {
                float RandScale = FMath::FRandRange(MinScaleSize, MaxScaleSize);
                NewActor->SetActorRelativeScale3D(FVector::OneVector * RandScale);
                NewActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
                SpawnedActors.Add(NewActor);
            }
        }
    }
}

void APropSpawner::ClearGeneratedActors()
{
    for (AActor* Actor : SpawnedActors)
    {
        if (Actor && IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    SpawnedActors.Empty();
}
