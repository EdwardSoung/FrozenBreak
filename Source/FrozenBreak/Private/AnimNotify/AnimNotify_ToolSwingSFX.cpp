// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_ToolSwingSFX.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Actor.h"





USoundBase* UAnimNotify_ToolSwingSFX::PickRandomSound() const
{
	const int32 Count = SwingSounds.Num();
	if (Count <= 0)
	{
		return nullptr;
	}

	int32 PickedIndex = FMath::RandRange(0, Count - 1);

	// 같은 소리 연속 방지
	if (bAvoidSameSoundTwice && Count > 1)
	{
		while (PickedIndex == LastPlayedIndex)
		{
			PickedIndex = FMath::RandRange(0, Count - 1);
		}
	}

	LastPlayedIndex = PickedIndex;
	return SwingSounds[PickedIndex];
}

void UAnimNotify_ToolSwingSFX::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	if (SkipChance > 0.0f)
	{
		const float Roll = FMath::FRand(); // 0.0 ~ 1.0
		if (Roll < SkipChance)
		{
			return;
		}
	}

	USoundBase* SoundToPlay = PickRandomSound();
	if (!SoundToPlay)
	{
		return;
	}

	// 부착 재생(따라다님)
	if (bAttachToMesh)
	{
		UGameplayStatics::SpawnSoundAttached(
			SoundToPlay,
			MeshComp,
			SocketName,
			LocationOffset,
			EAttachLocation::KeepRelativeOffset,
			true,
			VolumeMultiplier,
			PitchMultiplier
		);

		return;
	}

	// 월드 위치 재생
	FVector SpawnLocation = Owner->GetActorLocation();

	if (!SocketName.IsNone())
	{
		if (MeshComp->DoesSocketExist(SocketName))
		{
			SpawnLocation = MeshComp->GetSocketLocation(SocketName);
		}
	}

	SpawnLocation = SpawnLocation + LocationOffset;

	UGameplayStatics::PlaySoundAtLocation(
		Owner,
		SoundToPlay,
		SpawnLocation,
		VolumeMultiplier,
		PitchMultiplier
	);
}