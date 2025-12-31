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
#include <UI/Prop/PropDurabilityWidget.h>

ABuffableWorldProp::ABuffableWorldProp()
{
	BuffArea = CreateDefaultSubobject<USphereComponent>(TEXT("BuffArea"));
	BuffArea->SetupAttachment(RootComponent);
	BuffArea->SetSphereRadius(BuffAreaRadius);
	BuffArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABuffableWorldProp::OnSelect_Implementation(bool bIsStarted)
{
	bool MeetAvailable = false;
	if (!Meets.IsEmpty())
	{
		for (auto& SingleMeet : Meets)
		{
			if (SingleMeet->GetAmount() > 0)
			{
				MeetAvailable = true;
				break;
			}
		}
	}
	if (MeetAvailable) Super::OnSelect_Implementation(bIsStarted);
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
			EventSystem->Character.OnSendRawMeet.AddDynamic(this, &ABuffableWorldProp::SetMeets);
		}
		
		if (UWidget* Widget = DurabilityWidget->GetUserWidgetObject())
		{
			CampfireLifeBar = Cast<UPropDurabilityWidget>(DurabilityWidget->GetUserWidgetObject());
		}

		if (MaxDurability >= 0)
		{
			StartCampfireTimer();
		}
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
	if (OtherActor != GetOwner() && !IsFurnace) return;

	if (PropType == EPropType::Campfire)
	{
		if (CampfireBuffValues) GiveCampFireBuff();
		ReadyToCook();
	}
}

void ABuffableWorldProp::EndBuff(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor != GetOwner()) return;

	if (PropType == EPropType::Campfire && CampfireBuffValues)
	{
		FinishCamFireBuff();
	}
}

void ABuffableWorldProp::CampfireAction()
{
	if (!Meets.IsEmpty())
	{
		for (auto& SingleMeet : Meets)
		{
			if (SingleMeet->GetAmount() > 0)
			{
				EventSystem->Character.OnCookRequested.Broadcast(SingleMeet);
				IsCookValid();
				break;
			}
		}
	}
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
	if (CampfireLifeBar) 
	{
		DurabilityWidget->SetVisibility(true);
		CampfireLifeBar->SetVisibility(ESlateVisibility::Visible);
	}
	StatusCalculater->IncreaseTemperature(CampfireBuffValues->TemperatureBuffValue);
	StatusCalculater->IncreaseFatigue(CampfireBuffValues->FatigueBuffValue);
	StatusCalculater->IncreaseHunger(CampfireBuffValues->HungerBuffValue);

}

void ABuffableWorldProp::ReadyToCook()
{
	if (EventSystem)
	{
		EventSystem->Character.OnRequestIventoryRawMeet.Broadcast();
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
	if(CampfireLifeBar) CampfireLifeBar->SetVisibility(ESlateVisibility::Hidden);
}

void ABuffableWorldProp::StartCampfireTimer()
{
	CurrentDurability = 0.0f;

	if (IsValid(CampfireLifeBar))
	{
		CampfireLifeBar->SetDurabilityProgress(CurrentDurability);
	}

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(CampfireHandle);
		W->GetTimerManager().SetTimer(
			CampfireHandle,
			this,
			&ABuffableWorldProp::CampfireTick,
			CampfireTimerRate,
			true
		);
	}
}

void ABuffableWorldProp::CampfireTick()
{
	if (!IsValid(this) || IsActorBeingDestroyed())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(CampfireHandle);
		}
		return;
	}

	CurrentDurability += CampfireTimerRate;

	if (IsValid(CampfireLifeBar) &&
		CampfireLifeBar->GetVisibility() == ESlateVisibility::Visible)
	{
		CampfireLifeBar->SetDurabilityProgress(CurrentDurability / MaxDurability);
	}

	if (CurrentDurability >= MaxDurability)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(CampfireHandle);
		}
		Destroy();
	}
}
