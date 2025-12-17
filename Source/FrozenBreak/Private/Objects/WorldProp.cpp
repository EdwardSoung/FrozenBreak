// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/WorldProp.h"
#include "GameSystem/Events/StatusEvents.h"
#include "CommonComponents/StatComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Prop/InteractionWidget.h"
#include "Data/PropData.h"
#include "GameSystem/EventSubSystem.h"
#include "Interface/Interactable.h"

// Sets default values
AWorldProp::AWorldProp()
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
void AWorldProp::BeginPlay()
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
			// 데이터 에셋에 작성된 데이터를 StatComponent로 보낸다.
			StatComponent->InitStat(Data->Durability);
		}
	}
	else
	{
		// 이건 정말 끔찍한 일이야
		return;
	}
}

void AWorldProp::DoAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("WorldProp : 인터페이스 받았음"))
	// 주석 == ToDo
	// 어느정도 작성되면 타입별로 분리해 함수로 뺄 예정
	if (Data->PropType == EPropType::Tree)
	{
		// Player : 나무 베는 애님
		// Tree : 나무 베는 애님 노티파이에 맞춰 Durability가 깎여야 하고
		// Durability가 0이 되었을 시 Timber를 드랍? 인벤에 추가?
		// Durability가 0이 되었을 시 Tree 액터가 없어질 지??
		// 중복실행을 막아야 함
		UE_LOG(LogTemp, Log, TEXT("나무와 상호작용"));
		return;
	}
	if (Data->PropType == EPropType::Rock)
	{
		// Player : 채굴하는 애님
		// Rock : 채굴하는 애님 노티파이에 맞춰 Durability가 깎여야 하고
		// Durability가 0이 되었을 시 Stone를 드랍? 인벤에 추가?
		// Durability가 0이 되었을 시 Rock 액터가 없어질 지??
		// 중복실행을 막아야 함
		UE_LOG(LogTemp, Log, TEXT("바위와 상호작용"));
		return;
	}
	if (Data->PropType == EPropType::Bed)
	{
		// if (잘 수 있는 시간대가 있어야 한다)
		// Player : 침대에 눕는 애님은 없으니 위젯 애니메이션으로 검어졌다가 시간지나고 뭐 텍스트 띄우고...
		// Player 피로도를 회복 시켜줘야 한다.
		// 자고 일어났을 때 Player 배고픔을 깎을 지?
		// 중복실행을 막아야 함

		if (EventSystem)
		{
			EventSystem->Status.OnSetFatigue.Broadcast(FatigueRecoveryAmount);
			EventSystem->Status.OnSetHunger.Broadcast(HungerReductionAmount);
			UE_LOG(LogTemp, Log, TEXT("BroadCast 보냄."));
		}

		return;
	}
	if (Data->PropType == EPropType::CraftingTable)
	{
		// HUD : 제작대 UI를 띄워야 한다.
		// 중복실행을 막아야 함
		UE_LOG(LogTemp, Log, TEXT("제작대와 상호작용"));
		return;
	}
	if (Data->PropType == EPropType::Campfire)
	{
		// HUD : 요리 UI를 띄워야 한다.
		// 중복실행을 막아야 함
		UE_LOG(LogTemp, Log, TEXT("모닥불과 상호작용"));
		return;
	}


	UE_LOG(LogTemp, Warning, TEXT("프롭 데이터가 세팅되어 있지 않습니다."));

}

void AWorldProp::OnSelect_Implementation(bool bIsStarted)
{
	// 라인 트레이스에 맞은 시점에 위젯 텍스트를 업데이트 한 뒤, 보여준다.
	if (auto Widget = Cast<UInteractionWidget>(InteractionWidget->GetUserWidgetObject()))
	{
		Widget->UpdateInteraction(Data->PropType, Data->InteractionKey);
		InteractionWidget->SetVisibility(bIsStarted);
	}
}
