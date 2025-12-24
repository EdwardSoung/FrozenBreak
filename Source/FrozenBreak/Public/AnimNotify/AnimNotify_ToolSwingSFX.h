// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ToolSwingSFX.generated.h"

/**
 * 
 */

class USoundBass;

UENUM(BluePrintType)
enum class EToolSwingSoundType : uint8
{
	Auto UMETA(DisplayName = "Auto (Single Sound)"),
	Axe  UMETA(DisplayName = "Axe"),
	Pickaxe UMETA(DisplayName = "Pickaxe")
};

UCLASS()
class FROZENBREAK_API UAnimNotify_ToolSwingSFX : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	// 사운드 3개(또는 그 이상) 넣어두고 랜덤으로 1개 재생
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolSwingSFX|Sound")
	TArray<TObjectPtr<USoundBase>> SwingSounds;

	// 같은 소리 연속 재생 방지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolSwingSFX|Sound")
	bool bAvoidSameSoundTwice = true;

	// 가끔 소리 출력 안함(자연스럽게)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolSwingSFX|Sound", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SkipChance = 0.3f;

	// 볼륨/피치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolSwingSFX", meta = (ClampMin = "0.0"))
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolSwingSFX", meta = (ClampMin = "0.0"))
	float PitchMultiplier = 1.0f;

	// true면 MeshComp에 붙여서 재생(따라다님), false면 월드 위치에서 재생
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolSwingSFX")
	bool bAttachToMesh = false;

	// 소켓 이름(비우면 컴포넌트/액터 위치 기준)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolSwingSFX")
	FName SocketName = NAME_None;

	// 위치 오프셋(월드 위치/부착 재생 둘 다 적용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolSwingSFX")
	FVector LocationOffset = FVector::ZeroVector;

	// (선택) Owner가 특정 Tag를 가지고 있을 때만 재생
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolSwingSFX")
	bool bOnlyIfOwnerHasTag = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolSwingSFX", meta = (EditCondition = "bOnlyIfOwnerHasTag"))
	FName RequiredOwnerTag = NAME_None;

private:
	USoundBase* PickRandomSound() const;

	// Notify 인스턴스 안에서 마지막 인덱스 기억(연속 중복 방지용)
	mutable int32 LastPlayedIndex = INDEX_NONE;
	
};
