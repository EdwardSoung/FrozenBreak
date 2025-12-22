// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BuffableWorldProp.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "CommonComponents/StatComponent.h"
#include "GameSystem/StatusCalculationSubSystem.h"
#include "GameSystem/EventSubSystem.h"
#include "Data/StatBuffValueData.h"
#include "Data/PropData.h"
#include "UI/Prop/InteractionWidget.h"

ABuffableWorldProp::ABuffableWorldProp()
{
	BuffArea = CreateDefaultSubobject<USphereComponent>(TEXT("BuffArea"));
	BuffArea->SetupAttachment(RootComponent);
	BuffArea->SetSphereRadius(BuffAreaRadius);
	BuffArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABuffableWorldProp::OnSelect_Implementation(bool bIsStarted)
{
	if (Meets.IsEmpty() || Meets[0]->GetAmount() <= 0)
	{
		return;
	}
	Super::OnSelect_Implementation(bIsStarted);
}

void ABuffableWorldProp::BeginPlay()
{
	Super::BeginPlay();

	StatusCalculater = GetWorld()->GetSubsystem<UStatusCalculationSubSystem>();

	BuffArea->OnComponentBeginOverlap.AddDynamic(this, &ABuffableWorldProp::BeginBuff);
	BuffArea->OnComponentEndOverlap.AddDynamic(this, &ABuffableWorldProp::EndBuff);

	if (Data) PropType = Data->PropType;
	

	if (PropType == EPropType::Campfire)
	{
		if (EventSystem)
		{
			EventSystem->Chraracter.OnSendRawMeet.AddDynamic(this, &ABuffableWorldProp::SetMeets);
		}
		StartCampfireTimer();
	}
}

void ABuffableWorldProp::BeginBuff(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (PropType == EPropType::Campfire && CampfireBuffValues)
	{
		GiveCampFireBuff();
		ReadyToCook();
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

void ABuffableWorldProp::CampfireAction()
{
	if (!Meets.IsEmpty() && Meets[0]->GetAmount() > 0)
	{
		EventSystem->Chraracter.OnCookRequested.Broadcast(Meets[0]);
	}
	IsCookValid();
}

void ABuffableWorldProp::IsCookValid()
{
	if (auto Widget = Cast<UInteractionWidget>(InteractionWidget->GetUserWidgetObject()))
	{
		Widget->UpdateInteraction(Data->PropType, Data->InteractionKey);
		InteractionWidget->SetVisibility(!Meets.IsEmpty() && Meets[0]->GetAmount() > 0);
	}
}

void ABuffableWorldProp::GiveCampFireBuff()
{
	StatusCalculater->IncreaseTemperature(CampfireBuffValues->TemperatureBuffValue);
	StatusCalculater->IncreaseFatigue(CampfireBuffValues->FatigueBuffValue);
	StatusCalculater->IncreaseHunger(CampfireBuffValues->HungerBuffValue);
}

void ABuffableWorldProp::ReadyToCook()
{
	if (EventSystem)
	{
		EventSystem->Chraracter.OnRequestIventoryRawMeet.Broadcast();
	}
}

void ABuffableWorldProp::SetMeets(TArray<UInventoryItem*> InData)
{
	if (!InData.IsEmpty())
	{
		for (auto& SingleMeet : InData)
		{
			Meets.Add(SingleMeet);
		}
	}
}

void ABuffableWorldProp::FinishCamFireBuff()
{
	StatusCalculater->DecreaseTemperature(CampfireBuffValues->TemperatureBuffValue);
	StatusCalculater->DecreaseFatigue(CampfireBuffValues->FatigueBuffValue);
	StatusCalculater->DecreaseHunger(CampfireBuffValues->HungerBuffValue);
}

void ABuffableWorldProp::StartCampfireTimer()
{
	StatComponent->CurrentHealth = 0;
	GetWorld()->GetTimerManager().SetTimer(
		CampfireHandle,
		[this]() {
			StatComponent->CurrentHealth += CampfireTimerRate;
			if (StatComponent->CurrentHealth >= StatComponent->MaxHealth)
			{
				GetWorld()->GetTimerManager().ClearTimer(CampfireHandle);
				Destroy();
			}
		},
		CampfireTimerRate,
		true
	);
}
