// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BuffableWorldProp.h"
#include "Components/SphereComponent.h"
#include "GameSystem/StatusCalculationSubSystem.h"
#include "Data/StatBuffValueData.h"
#include "Data/PropData.h"

ABuffableWorldProp::ABuffableWorldProp()
{
	BuffArea = CreateDefaultSubobject<USphereComponent>(TEXT("BuffArea"));
	BuffArea->SetupAttachment(RootComponent);
	BuffArea->SetSphereRadius(BuffAreaRadius);
	BuffArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABuffableWorldProp::BeginPlay()
{
	StatusCalculater = GetWorld()->GetSubsystem<UStatusCalculationSubSystem>();

	BuffArea->OnComponentBeginOverlap.AddDynamic(this, &ABuffableWorldProp::BeginBuff);
	BuffArea->OnComponentEndOverlap.AddDynamic(this, &ABuffableWorldProp::EndBuff);
}

void ABuffableWorldProp::BeginBuff(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (Data) PropType = Data->PropType;

	if (PropType == EPropType::Campfire && CampfireBuffValues)
	{
		GiveCampFireBuff();
	}
}

void ABuffableWorldProp::EndBuff(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (PropType == EPropType::Campfire && CampfireBuffValues)
	{
		FinishCamFireBuff();
	}
}

void ABuffableWorldProp::GiveCampFireBuff()
{
	StatusCalculater->IncreaseTemperature(CampfireBuffValues->TemperatureBuffValue);
	StatusCalculater->IncreaseFatigue(CampfireBuffValues->FatigueBuffValue);
	StatusCalculater->IncreaseHunger(CampfireBuffValues->HungerBuffValue);
}

void ABuffableWorldProp::FinishCamFireBuff()
{
	StatusCalculater->DecreaseTemperature(CampfireBuffValues->TemperatureBuffValue);
	StatusCalculater->DecreaseFatigue(CampfireBuffValues->FatigueBuffValue);
	StatusCalculater->DecreaseHunger(CampfireBuffValues->HungerBuffValue);
}
