// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PickupItem.h"
#include "Kismet/GameplayStatics.h"

#include "Data/ItemData.h"

#include "PlayerComponents/InventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "CommonComponents/StatComponent.h"

#include "UI/Prop/InteractionWidget.h"
#include "GameSystem/EventSubSystem.h"


// Sets default values
APickupItem::APickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat Component"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	// 트레이스가 맞도록 쿼리 활성화
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 플레이어(캐릭터) 채널 무시
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	// 인터랙션 트레이스 채널 Block (InteractionComponent와 동일 채널)
	Mesh->SetCollisionResponseToChannel(InteractableActorChannel, ECR_Block);
	// 물리 시뮬레이션 활성화
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TextWidget"));
	InteractionWidget->SetupAttachment(Mesh);
	InteractionWidget->SetRelativeLocation(FVector(0, 0, 0));
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetVisibility(false);
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();
		
	EventSystem = UEventSubSystem::Get(this);
	if (!EventSystem)
	{
		UE_LOG(LogTemp, Log, TEXT("EventSystem이 설정되어 있지 않다."));
	}

	if (StatComponent)
	{
		if (Data)
		{
			// Attack을 가지지 않은 아이템의 Attack 기본값 (0)
			float Attack = 0.f;
			if (const float* AttackStats = Data->Stats.Find(EItemStatType::Attack))
			{
				Attack = *AttackStats;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("아이템에 Attack 스텟이 설정되어 있지 않거나, 필요없는 아이템 입니다."));
			}
			// 데이터 에셋에 작성된 데이터를 StatComponent로 보낸다.
			StatComponent->InitStat(Data->Durability, Attack);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup Item Begin : 아이템 데이터가 없음!"));
		}
	}
}

void APickupItem::DoAction_Implementation() // 아이템을 획득
{
	if (Data)
	{
		// 플레이어 Inventory Component에 Item List가 세팅되어 있어야 한다.
		EventSystem->Character.OnGetPickupItem.Broadcast(Data->ItemType, 1);

		SetActorHiddenInGame(true);
		SetLifeSpan(0.001f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("아이템 데이터가 세팅되어 있지 않습니다."));
	}
}

void APickupItem::OnSelect_Implementation(bool bIsStarted)
{
	// 라인 트레이스에 맞은 시점에 위젯 텍스트를 업데이트 한 뒤, 보여준다.
	if (auto Widget = Cast<UInteractionWidget>(InteractionWidget->GetUserWidgetObject()))
	{
		Widget->UpdateInteraction(Data->ItemType, Data->InteractionKey);
		InteractionWidget->SetVisibility(bIsStarted);
	}
}

