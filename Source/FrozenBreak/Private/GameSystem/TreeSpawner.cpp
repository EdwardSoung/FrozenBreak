// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/TreeSpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATreeSpawner::ATreeSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
    RootComponent = SpawnVolume;

    // 박스 크기 기본값 (가로세로 10미터)
    //SpawnVolume->SetBoxExtent(FVector(1000.f, 1000.f, 100.f));
    SpawnVolume->SetLineThickness(5.0f); // 에디터에서 잘 보이게

    SpawnActors();
}

void ATreeSpawner::SpawnActors()
{
    if (!ActorToSpawn) return;

    // 박스의 원점과 크기(반지름 개념) 가져오기
    FVector Origin = SpawnVolume->GetComponentLocation();
    FVector BoxExtent = SpawnVolume->GetScaledBoxExtent();

    for (int32 i = 0; i < SpawnCount; i++)
    {
        // 1. 박스 범위 안에서 랜덤한 X, Y 좌표 뽑기
        // (Z는 위에서 아래로 쏠 거라 박스 제일 위쪽 높이로 잡음)
        FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, BoxExtent);

        // 2. 바닥 찾기 (LineTrace)
        // 박스 천장에서 바닥까지 레이저를 쏩니다.
        FVector TraceStart = RandomPoint;
        TraceStart.Z = Origin.Z + BoxExtent.Z; // 박스 천장

        FVector TraceEnd = RandomPoint;
        TraceEnd.Z = Origin.Z - BoxExtent.Z - 1000.0f; // 박스 바닥보다 넉넉하게 아래로

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this); // 나는 무시

        // 바닥에 닿았는가? (WorldStatic 채널 기준)
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            TraceStart,
            TraceEnd,
            ECC_WorldStatic,
            Params
        );

        if (bHit)
        {
            // 3. 회전값 설정
            FRotator SpawnRotation = FRotator::ZeroRotator;
            if (bRandomYaw)
            {
                SpawnRotation.Yaw = FMath::RandRange(0.0f, 360.0f);
            }

            // 경사면에 맞춰 기울이고 싶다면?
            // SpawnRotation = HitResult.ImpactNormal.Rotation(); (선택사항)

            // 4. 스폰 실행
            AActor* NewActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, HitResult.Location, SpawnRotation);

            if (NewActor)
            {
                // 아웃라이너 더러워지니까 이 액터 폴더 아래로 정리
                NewActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
                SpawnedActors.Add(NewActor);
            }
        }
    }
}

void ATreeSpawner::ClearGeneratedActors()
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

