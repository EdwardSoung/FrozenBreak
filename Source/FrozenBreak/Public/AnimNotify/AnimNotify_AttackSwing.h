#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackSwing.generated.h"

class USoundBase;

UCLASS()
class FROZENBREAK_API UAnimNotify_AttackSwing : public UAnimNotify
{
	GENERATED_BODY()

public:
	//랜덤 재생할 휘두르기 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSwing|Sound")
	TArray<TObjectPtr<USoundBase>> SwingSounds;

	//트루면 직전과 같은 사운드 반복실행 안됨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSwing|Sound")
	bool bNoImmediateRepeat = true;


	//볼륨랜덤
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSwing|Sound", meta = (ClampMin = "0.0"))
	float VolumeMin = 0.90f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSwing|Random", meta = (ClampMin = "0.0"))
	float VolumeMax = 1.05f;

	// 피치 랜덤 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSwing|Random", meta = (ClampMin = "0.01"))
	float PitchMin = 0.95f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSwing|Random", meta = (ClampMin = "0.01"))
	float PitchMax = 1.05f;

	// true면 사운드가 nullptr인 항목은 뽑지 않도록 여러 번 재시도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSwing|Random")
	bool bRetryIfNull = true;

	// nullptr이면 Mesh 위치에서 재생
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSwing|Location")
	FName PlaySocketName;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
private:
	//마지막 인덱스 기억해 두고 나중에 연속되는 패턴 줄이기
	static TMap<TWeakObjectPtr<USkeletalMeshComponent>, int32> LastPlayedIndexByMesh;

	int32 PickIndex_NoRepeat(int32 Count, int32 LastIndex) const;
	bool IsValidRange(float Min, float Max) const { return Max >= Min; }
};
