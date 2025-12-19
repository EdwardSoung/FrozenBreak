// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FootStep.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Player/ActionCharacter.h"



static USoundBase* PickRandomNoRepeat(const TArray<USoundBase*>& Sounds, int32& InOutLastIndex)
{
	const int32 Count = Sounds.Num();
	if (Count <= 0)
		return nullptr;

	// 소리가 1개면 그냥 그거
	if (Count == 1)
	{
		InOutLastIndex = 0;
		return Sounds[0];
	}

	// 2개 이상이면 직전 인덱스 제외
	int32 Index = FMath::RandRange(0, Count - 1);

	if (Index == InOutLastIndex)
	{
		// 한 칸 옆으로 밀어서 무조건 다르게
		Index = (Index + 1) % Count;
	}

	InOutLastIndex = Index;
	return Sounds[Index];
}
void UAnimNotify_FootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}
	UWorld* World = MeshComp->GetWorld();
	if (!World)
	{
		return;
	}

	if (World->WorldType == EWorldType::Editor ||
		World->WorldType == EWorldType::EditorPreview ||
		World->WorldType == EWorldType::Inactive)
	{
		return;
	}

	AActionCharacter* Character = Cast <AActionCharacter> (MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	const TArray<USoundBase*>* SoundList = nullptr;
	int32* LastIndexPtr = nullptr;

	

	if (FootstepSide == EFootstepSide::Left)//좌우로 다른 사운드를 배치하고 싶음, 배열에 따라 한 3개정도 사운드중 하나가 랜덤으로 재생될거임
	{
		SoundList = &Character->FootstepSounds_L;
		LastIndexPtr = &Character->LastFootstepIndex_L;
	}
	else
	{
		SoundList = &Character->FootstepSounds_R;
		LastIndexPtr = &Character->LastFootstepIndex_R;
	}
	
	if (!SoundList|| !LastIndexPtr)
	{
		return;
	}
	USoundBase* FootstepSound = PickRandomNoRepeat(*SoundList, *LastIndexPtr);

	if (!FootstepSound)
	{
		return;
	}

	
	const float BaseVolume = 0.1f;

	// 걷기/뛰기 가중치)
	float Speed = Character->GetVelocity().Size();

	// 0.0~1.0 알파 만들고
	float Clamped = FMath::Clamp(Speed, 120.0f, 650.0f);
	float Alpha = (Clamped - 120.0f) / (650.0f - 120.0f);

	// 걷기 0.9배 ~ 뛰기 1.3배 정도만
	float Volume = BaseVolume * FMath::Lerp(0.9f, 1.3f, Alpha);

	// 최종 안전 clamp
	Volume = FMath::Clamp(Volume, 0.12f, 0.38f);
	float Pitch = FMath::RandRange(0.95f, 1.05f); // 자연스럽게 

	// 발 쪽에 소켓 달아서 거기서 소리 출력 시키기
	FName FootSocketName =
		(FootstepSide == EFootstepSide::Left) 
		? TEXT("Footstep_L") 
		: TEXT("Footstep_R");

	FVector SoundLocation = MeshComp->GetSocketLocation(FootSocketName);

	UGameplayStatics::PlaySoundAtLocation(
		Character,
		FootstepSound,
		SoundLocation,
		Volume,
		Pitch
	);

}
