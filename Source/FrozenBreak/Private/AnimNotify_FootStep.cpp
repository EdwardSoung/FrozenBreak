// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FootStep.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "Player/ActionCharacter.h"





static USoundBase* PickRandomNoRepeat(
	const TArray<TObjectPtr<USoundBase>>& Sounds,
	int32& InOutLastIndex
)
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
	return Sounds[Index].Get();
}


void UAnimNotify_FootStep::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	UE_LOG(LogTemp, Error, TEXT("🔥🔥 FOOTSTEP NOTIFY FIRED"));

	if (!MeshComp)
	{
		return;
	}
	UWorld* World = MeshComp->GetWorld();
	if (!World)
	{
		return;
	}
	//터짐 방지용 프리뷰 월드에서는 실행 안되게 
	//if (World->WorldType == EWorldType::Editor ||
	//	World->WorldType == EWorldType::EditorPreview ||
	//	World->WorldType == EWorldType::Inactive)
	//{
	//	return;
	//}

	AActionCharacter* Character = Cast <AActionCharacter> (MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	// 발 쪽에 소켓 달아서 거기서 소리 출력 시키기
	FName FootSocketName =
		(FootstepSide == EFootstepSide::Left)
		? TEXT("Footstep_L")
		: TEXT("Footstep_R");

	FVector SoundLocation = MeshComp->GetSocketLocation(FootSocketName);

	//발밑 서페이스 타입 구하기
	FVector Start = SoundLocation + FVector(0, 0, 20); 
	FVector End = SoundLocation - FVector(0, 0, 60); 

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);



	//바닥에 트레이스 쏘기
	bool bHit = World->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	EPhysicalSurface Surface = SurfaceType_Default;

	if (bHit)
	{
		Character->LastFootSurface = Surface;
	}
	else
	{
		Surface = Character->LastFootSurface;
	}

	if (Hit.PhysMaterial.IsValid())
	{
		Surface = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
	}
	else if (Hit.GetComponent() && Hit.GetComponent()->GetBodyInstance())
	{
		UPhysicalMaterial* SimplePM =
			Hit.GetComponent()->GetBodyInstance()->GetSimplePhysicalMaterial();

		if (SimplePM)
		{
			Surface = UPhysicalMaterial::DetermineSurfaceType(SimplePM);
		}
	}
	//디버그용
	DrawDebugLine(World, Start, End, bHit ? FColor::Green : FColor::Red, false, 0.2f, 0, 1.0f);

	const FString OwnerName = Character ? Character->GetName() : TEXT("None");
	const FString AnimName = Animation ? Animation->GetName() : TEXT("None");
	const FString HitActorName = Hit.GetActor() ? Hit.GetActor()->GetName() : TEXT("None");

	UE_LOG(LogTemp, Warning, TEXT("[Footstep] Owner=%s Anim=%s Side=%s"),
		*OwnerName,
		*AnimName,
		(FootstepSide == EFootstepSide::Left) ? TEXT("L") : TEXT("R"));

	UE_LOG(LogTemp, Warning, TEXT("[Footstep] Socket=%s Loc=%s Start=%s End=%s"),
		*FootSocketName.ToString(),
		*SoundLocation.ToString(),
		*Start.ToString(),
		*End.ToString());

	UE_LOG(LogTemp, Warning, TEXT("[Footstep] TraceHit=%d HitActor=%s PhysMat=%s Surface=%d"),
		bHit ? 1 : 0,
		*HitActorName,
		Hit.PhysMaterial.IsValid() ? *Hit.PhysMaterial->GetName() : TEXT("None"),
		(int32)Surface);

	UE_LOG(LogTemp, Warning, TEXT(
		"[Footstep] HitComp=%s CompPhysMat=%s"),
		Hit.GetComponent() ? *Hit.GetComponent()->GetName() : TEXT("None"),
		(Hit.GetComponent() && Hit.GetComponent()->GetBodyInstance())
		? *GetNameSafe(Hit.GetComponent()->GetBodyInstance()->GetSimplePhysicalMaterial())
		: TEXT("None")
	);



	const FFootstepSoundList* Found = nullptr;
	int32* LastIndexPtr = nullptr;
	// 기본배열 먼저 잡아둠
	const FFootstepSoundList* DefaultFound = nullptr;

	if (FootstepSide == EFootstepSide::Left)//좌우로 다른 사운드를 배치하고 싶음, 배열에 따라 한 3개정도 사운드중 하나가 랜덤으로 재생될거임
	{
		Found = Character->FootstepBySurface_L.Find(Surface);
		LastIndexPtr = &Character->LastFootstepIndex_L;


	}
	else
	{
		Found = Character->FootstepBySurface_R.Find(Surface);
		LastIndexPtr = &Character->LastFootstepIndex_R;
	}
	//못찾겠으면 롤백
	if (!Found || Found->Sounds.Num() == 0)
	{
		Found = DefaultFound;
	}
	if (!Found || Found->Sounds.Num() == 0 || !LastIndexPtr)
	{
		return;
	}

	USoundBase* FootstepSound = PickRandomNoRepeat(Found->Sounds, *LastIndexPtr);
	if (!FootstepSound)
	{
		return;
	}

	
	const float BaseVolume = 0.25f;
	
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

	

	UGameplayStatics::PlaySoundAtLocation(
		Character,
		FootstepSound,
		SoundLocation,
		Volume,
		Pitch
	);

	

}
