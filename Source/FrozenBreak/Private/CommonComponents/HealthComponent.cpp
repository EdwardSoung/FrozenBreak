// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonComponents/HealthComponent.h"
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

		if (UEventSubSystem* EventSystem = UEventSubSystem::GetEventSystem(this))
		{
			EventSystem->Status.OnHealthPointChanged.Broadcast(CurrentHealth / MaxHealth);
		}
	}
	else
	{
		// TODO : 캐릭터 사망
	}
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	InitStatus();
}

void UHealthComponent::InitStatus()
{
	// 체력 초기 값 세팅
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