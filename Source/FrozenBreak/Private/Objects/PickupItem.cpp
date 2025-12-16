// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PickupItem.h"
#include "Data/ItemData.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "CommonComponents/StatComponent.h"
#include "UI/Prop/InteractionWidget.h"

// Sets default values
APickupItem::APickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat Component"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// 트레이스가 맞도록 쿼리 활성화
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 인터랙션 트레이스 채널 Block (InteractionComponent와 동일 채널)
	Mesh->SetCollisionResponseToChannel(InteractableActorChannel, ECR_Block);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TextWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetRelativeLocation(FVector(0, 0, 0));
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetVisibility(false);
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();
		
	if (StatComponent)
	{
		if (Data)
		{
			// Attack을 가지지 않은 아이템의 Attack 기본값 (0)
			float Attack = 0.f;
			if (const float* AttackStats = Data->Stats.Find(EItemStatType::Attack))
			{
				// 이 아이템에게 Attack Stats이 설정되어 있다 (에디터에서 설정)
				Attack = *AttackStats;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("아이템에 Attack 스텟이 설정되어 있지 않거나, 필요없는 아이템 입니다."));
			}
			StatComponent->InitStat(Data->Durability, Attack);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup Item Begin : 아이템 데이터가 없음!"));
		}
	}
}

void APickupItem::DoAction_Implementation()
{
	//마우스 키 누르는 등의 액션이 취해짐
	if (Data)
	{
		UE_LOG(LogTemp, Log, TEXT("뭔가 상호작용이 일어나야 한다."));

		// Test용 액터 제거
		SetActorHiddenInGame(true);

		// 타이밍 문제로 null 참조가 일어나서 조금 늦게 제거
		SetLifeSpan(0.001f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("아이템 데이터가 세팅되어 있지 않습니다."));
	}
}

void APickupItem::OnSelect_Implementation(bool bIsStarted)
{
	if (auto Widget = Cast<UInteractionWidget>(InteractionWidget->GetUserWidgetObject()))
	{
		Widget->UpdateInteraction(Data->ItemType, Data->InteractionKey);
		InteractionWidget->SetVisibility(bIsStarted);
	}
}

