// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_AttackSwing.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

TMap<TWeakObjectPtr<USkeletalMeshComponent>, int32> UAnimNotify_AttackSwing::LastPlayedIndexByMesh;

int32 UAnimNotify_AttackSwing::PickIndex_NoRepeat(int32 Count, int32 LastIndex) const
{
	if (Count <= 0)
		return INDEX_NONE;

	if (Count == 1)
		return 0;

	int32 Picked = FMath::RandRange(0, Count - 1);

	if (bNoImmediateRepeat && Picked == LastIndex)
	{
		// 한 번 더 뽑아보고, 그래도 같으면 옆으로
		Picked = FMath::RandRange(0, Count - 1);
		if (Picked == LastIndex)
		{
			Picked = (LastIndex + 1) % Count;
		}
	}

	return Picked;
}

void UAnimNotify_AttackSwing::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp)
		return;

	UWorld* World = MeshComp->GetWorld();
	if (!World)
		return;

	const int32 Count = SwingSounds.Num();
	if (Count <= 0)
		return;

	int32 LastIndex = INDEX_NONE;
	if (int32* Found = LastPlayedIndexByMesh.Find(MeshComp))
	{
		LastIndex = *Found;
	}

	// 인덱스 선택
	int32 PickedIndex = PickIndex_NoRepeat(Count, LastIndex);

	// nullptr 회피 옵션이 켜져 있으면 몇 번 재시도
	USoundBase* PickedSound = nullptr;

	if (bRetryIfNull)
	{
		const int32 MaxTries = FMath::Clamp(Count, 1, 10);
		for (int32 Try = 0; Try < MaxTries; ++Try)
		{
			if (PickedIndex == INDEX_NONE)
				return;

			PickedSound = SwingSounds[PickedIndex].Get();
			if (PickedSound)
				break;

			// 다시 뽑기 (마지막 인덱스는 유지)
			PickedIndex = FMath::RandRange(0, Count - 1);
			if (bNoImmediateRepeat && PickedIndex == LastIndex && Count > 1)
			{
				PickedIndex = (LastIndex + 1) % Count;
			}
		}
	}
	else
	{
		if (PickedIndex == INDEX_NONE)
			return;

		PickedSound = SwingSounds[PickedIndex].Get();
	}

	if (!PickedSound)
		return;

	// 볼륨/피치 랜덤
	const float Volume = IsValidRange(VolumeMin, VolumeMax) ? FMath::FRandRange(VolumeMin, VolumeMax) : 1.0f;
	const float Pitch = IsValidRange(PitchMin, PitchMax) ? FMath::FRandRange(PitchMin, PitchMax) : 1.0f;

	// 재생 위치: 소켓 지정했으면 소켓, 아니면 Mesh 위치
	FVector PlayLoc = MeshComp->GetComponentLocation();
	if (!PlaySocketName.IsNone() && MeshComp->DoesSocketExist(PlaySocketName))
	{
		PlayLoc = MeshComp->GetSocketLocation(PlaySocketName);
	}

	UGameplayStatics::PlaySoundAtLocation(World, PickedSound, PlayLoc, Volume, Pitch);

	// 마지막 인덱스 저장(연속 중복 줄이기)
	LastPlayedIndexByMesh.Add(MeshComp, PickedIndex);

	// 가끔 무효 키 정리 (메모리 쓰레기 방지)
	if (LastPlayedIndexByMesh.Num() > 64)
	{
		for (auto It = LastPlayedIndexByMesh.CreateIterator(); It; ++It)
		{
			if (!It.Key().IsValid())
			{
				It.RemoveCurrent();
			}
		}
	}
}