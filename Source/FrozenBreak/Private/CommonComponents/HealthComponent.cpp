// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonComponents/HealthComponent.h"
#include "Components/AudioComponent.h"
#include "GameSystem/EventSubSystem.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::SetPlayerHealth(float InHealthValue)
{
	if (CurrentHealth > 0)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth + InHealthValue, DefaultMinimum, MaxHealth);

		if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
		{
			EventSystem->Status.OnHealthPointChanged.Broadcast(CurrentHealth / MaxHealth);

			if (CurrentHealth <= 0)
			{
				EventSystem->Character.OnPlayerDead.Broadcast();

				return;
			}
		}

		const float HealthRatio = (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;

		// 20% 미만 진입
		if (!bIsLowHealth && HealthRatio <= LowHealthThreshold)
		{
			bIsLowHealth = true;

			if (LowHealthHeartbeatSound && HeartbeatAudioComponent)
			{
				HeartbeatAudioComponent->SetSound(LowHealthHeartbeatSound);
				HeartbeatAudioComponent->FadeIn(0.25f, 1.0f);
			}
		}
		// 20% 이상 회복
		else if (bIsLowHealth && HealthRatio > LowHealthThreshold)
		{
			bIsLowHealth = false;

			if (HeartbeatAudioComponent && HeartbeatAudioComponent->IsPlaying())
			{
				HeartbeatAudioComponent->FadeOut(0.25f, 0.0f);
			}
		}

		//저체력 상태면 “체력에 따라” 볼륨/피치 계속 갱신
		if (bIsLowHealth && HeartbeatAudioComponent && HeartbeatAudioComponent->IsPlaying())
		{
			// HealthRatio: 0.2 -> 0.0 로 내려갈수록 강해져야 함
			// t: 0 (0.2일 때) ~ 1 (0.0일 때)
			const float t = FMath::Clamp((LowHealthThreshold - HealthRatio) / LowHealthThreshold, 0.0f, 1.0f);

			const float NewVolume = FMath::Lerp(HeartbeatVolumeMin, HeartbeatVolumeMax, t);
			const float NewPitch = FMath::Lerp(HeartbeatPitchMin, HeartbeatPitchMax, t);

			HeartbeatAudioComponent->SetVolumeMultiplier(NewVolume);
			HeartbeatAudioComponent->SetPitchMultiplier(NewPitch);
		}
	}

	
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	InitStatus();
	BindStatusSettingEvents();

	HeartbeatAudioComponent = NewObject<UAudioComponent>(this);
	if (HeartbeatAudioComponent)
	{
		HeartbeatAudioComponent->bAutoActivate = false;
		HeartbeatAudioComponent->SetSound(LowHealthHeartbeatSound);
		HeartbeatAudioComponent->RegisterComponent();
	}
}

void UHealthComponent::InitStatus()
{
	// 체력 초기 값 세팅
	MaxHealth = DefaultMaxHealth;
	CurrentHealth = MaxHealth;

	/*
	플레이 시작 시 체력 UI 값을 초기화 하기 위한 코드. 타이밍 상 아래 코드가 실행되는 시점에 Widget의 바인딩이 안된 상태라
	이벤트를 받을수가 없음. 아래 코드는 현재는 쓰임이 없어 주석처리. Widget에서는 별도의 처리를 통해 UI 초기화.
	*/
	//if (UEventSubSystem* EventSystem = UEventSubSystem::GetEventSystem(this))
	//{
	//	EventSystem->Status.OnHealthPointChanged.Broadcast(CurrentHealth / MaxHealth);
	//}
}

void UHealthComponent::BindStatusSettingEvents()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		EventSystem->Status.OnSetHealth.AddDynamic(this, &UHealthComponent::SetPlayerHealth);
	}
}