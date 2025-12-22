// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/EscapeProp.h"
#include "CommonComponents/HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Gamesystem/EventSubSystem.h"
#include <Player/ActionCharacter.h>
#include <Kismet/GameplayStatics.h>
#include "UI/Prop/InteractionWidget.h"
#include "UI/Prop/PropDurabilityWidget.h"
#include "Data/PropData.h"

// Sets default values
AEscapeProp::AEscapeProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	
	SetRootComponent(MeshComponent);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TextWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetRelativeLocation(FVector(0, 0, 0));
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);	InteractionWidget->SetVisibility(false);


	DurabilityWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DurabilityWidget"));
	DurabilityWidget->SetupAttachment(RootComponent);
	DurabilityWidget->SetRelativeLocation(FVector(0, 0, 50));
	DurabilityWidget->SetWidgetSpace(EWidgetSpace::Screen);
	DurabilityWidget->SetVisibility(false);
}

// Called when the game starts or when spawned
void AEscapeProp::BeginPlay()
{
	Super::BeginPlay();	
}

void AEscapeProp::OnDamage(float InDamage)
{
	HealthComponent->OnDamaged(InDamage);
}

void AEscapeProp::RockAction()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		if (AActionCharacter* Player = Cast<AActionCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
		{
			if (auto Durability = Cast<UPropDurabilityWidget>(DurabilityWidget->GetUserWidgetObject()))
			{
				Durability->SetDurabilityProgress(FMath::Clamp(HealthComponent->CurrentHealth / HealthComponent->MaxHealth, 0.f, 1.f));
			}
			ToolAtkPower = Player->GetCurrentToolAtkPower();

			// 도구의 공격력 만큼 데미지 주기
			HealthComponent->OnDamaged(ToolAtkPower);

			// 플레이어 피로도 감소? 시키기
			EventSystem->Status.OnSetFatigue.Broadcast(FatigueCostPerWork);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Player 캐스팅 실패"));
		}
	}

	if (HealthComponent->CurrentHealth <= 0)
	{		
	}

}

void AEscapeProp::OnDead_Implementation()
{
	//음...탈출 성공 연출...
}

void AEscapeProp::DoAction_Implementation()
{
	RockAction();
}

void AEscapeProp::OnSelect_Implementation(bool bIsStart)
{

	if (auto Widget = Cast<UInteractionWidget>(InteractionWidget->GetUserWidgetObject()))
	{
		Widget->UpdateInteraction(Data->PropType, Data->InteractionKey);
		InteractionWidget->SetVisibility(bIsStart);
	}
}

