// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/PropSpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h" // 디버그 라인 그리기 위해 필요

// Sets default values
APropSpawner::APropSpawner()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
    RootComponent = SpawnVolume;

    SpawnVolume->SetLineThickness(5.0f); // 에디터에서 잘 보이게

}


void APropSpawner::SpawnActors()
{
    if (ActorToSpawn.Num() <= 0) return;

    // 박스의 원점과 크기(반지름 개념) 가져오기
    FVector Origin = SpawnVolume->GetComponentLocation();
    FVector BoxExtent = SpawnVolume->GetScaledBoxExtent();

    for (int32 i = 0; i < SpawnCount; i++)
    {
        // 1. 박스 범위 안에서 랜덤한 X, Y 좌표 뽑기
        float RandomX = FMath::RandRange(Origin.X - BoxExtent.X, Origin.X + BoxExtent.X);
        float RandomY = FMath::RandRange(Origin.Y - BoxExtent.Y, Origin.Y + BoxExtent.Y);

        FVector TraceStart = FVector(RandomX, RandomY, Origin.Z + BoxExtent.Z);
        FVector TraceEnd = FVector(RandomX, RandomY, Origin.Z - BoxExtent.Z - 2000.f); // 바닥보다 2000언리얼 유닛 더 아래까지 쏨


        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this); // 나는 무시

        // 바닥에 닿았는가?
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
            // 3. 회전값 설정
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
                // 아웃라이너 더러워지니까 이 액터 폴더 아래로 정리
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

FVector APropSpawner::GetRandomPointOnGround()
{
    FVector Origin = SpawnVolume->GetComponentLocation();
    FVector BoxExtent = SpawnVolume->GetScaledBoxExtent();

    // 1. X, Y만 랜덤으로 구함 (Z는 박스 최상단에서 시작할 것이므로 무시)
    float RandomX = FMath::RandRange(Origin.X - BoxExtent.X, Origin.X + BoxExtent.X);
    float RandomY = FMath::RandRange(Origin.Y - BoxExtent.Y, Origin.Y + BoxExtent.Y);

    // 2. 레이저 시작점: 박스의 가장 높은 곳 (Z Max)
    // 3. 레이저 끝점: 박스의 가장 낮은 곳보다 더 아래 (Z Min - 여유분)
    FVector Start = FVector(RandomX, RandomY, Origin.Z + BoxExtent.Z);
    FVector End = FVector(RandomX, RandomY, Origin.Z - BoxExtent.Z - 2000.f); // 바닥보다 2000언리얼 유닛 더 아래까지 쏨

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    // 4. 레이저 발사 (WorldStatic: 지형, 바닥 메쉬 등)
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_WorldStatic,
        QueryParams
    );

    // [디버깅] 눈으로 확인하기 (빨간선: 실패 / 초록선: 성공)
    // 게임 실행 후 이 선이 바닥을 뚫고 지나가는지 확인하세요.
    FColor LineColor = bHit ? FColor::Green : FColor::Red;
    DrawDebugLine(GetWorld(), Start, End, LineColor, false, 5.0f, 0, 2.0f);

    if (bHit)
    {
        return HitResult.Location;
    }

    // 바닥을 못 찾으면 0,0,0이라도 줘서 티가 나게 함 (혹은 Start 반환)
    return FVector::ZeroVector;
}
