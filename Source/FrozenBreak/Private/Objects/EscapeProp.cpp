// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/EscapeProp.h"

#include "CommonComponents/HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Gamesystem/EventSubSystem.h"
#include "GameSystem/FrozenForestGameState.h"

#include "UI/Prop/InteractionWidget.h"
#include "UI/Prop/PropDurabilityWidget.h"

#include "Data/PropData.h"

#include <Player/ActionCharacter.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AEscapeProp::AEscapeProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 인터랙션 트레이스 채널 Block (InteractionComponent와 동일 채널)
	MeshComponent->SetCollisionResponseToChannel(InteractableActorChannel, ECR_Block);

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
	
	if (Data)
	{
		MaxDurability = Data->Durability;
		CurrentDurability = MaxDurability;
	}
}

void AEscapeProp::RockAction()
{
	if (UEventSubSystem* EventSystem = UEventSubSystem::Get(this))
	{
		if (AActionCharacter* Player = Cast<AActionCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
		{
			if (auto Durability = Cast<UPropDurabilityWidget>(DurabilityWidget->GetUserWidgetObject()))
			{
				Durability->SetDurabilityProgress(FMath::Clamp(CurrentDurability / MaxDurability, 0.f, 1.f));
			}
			ToolAtkPower = Player->GetCurrentToolAtkPower();

			// 도구의 공격력 만큼 데미지 주기
			CurrentDurability -= ToolAtkPower;

			// 플레이어 피로도 감소? 시키기
			EventSystem->Status.OnSetFatigue.Broadcast(FatigueCostPerWork);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Player 캐스팅 실패"));
		}
	}

	if (CurrentDurability <= 0)
	{		
		auto State = GetWorld()->GetGameState<AFrozenForestGameState>();
		if (State)
		{
			State->SetGameState(EGameState::Success);
		}

		//바위 없애고 음...
		Destroy();
	}

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
	if (auto Durability = Cast<UPropDurabilityWidget>(DurabilityWidget->GetUserWidgetObject()))
	{
		DurabilityWidget->SetVisibility(bIsStart);
		Durability->SetDurabilityProgress(FMath::Clamp(CurrentDurability / MaxDurability, 0.f, 1.f));
	}
}

EPropType AEscapeProp::GetPropType() const
{
	return Data->PropType;
}

EItemType AEscapeProp::GetInteractableToolType() const
{
	return Data->InteractableToolType;
}
