// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/WorldProp.h"
#include "CommonComponents/StatComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Prop/InteractionWidget.h"
#include "Data/PropData.h"

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

	if (StatComponent)
	{
		if (Data)
		{
			StatComponent->InitStat(Data->Durability);
		}
	}
}

void AWorldProp::DoAction_Implementation()
{
	// 테스트
	if (Data->PropType == EPropType::Tree)
	{
		UE_LOG(LogTemp, Log, TEXT("나무와 상호작용"));
		return;
	}
	if (Data->PropType == EPropType::Rock)
	{
		UE_LOG(LogTemp, Log, TEXT("바위와 상호작용"));
		return;
	}
	if (Data->PropType == EPropType::Bed)
	{
		UE_LOG(LogTemp, Log, TEXT("침대와 상호작용"));
		return;
	}
	if (Data->PropType == EPropType::CraftingTable)
	{
		UE_LOG(LogTemp, Log, TEXT("제작대와 상호작용"));
		return;
	}
	if (Data->PropType == EPropType::Campfire)
	{
		UE_LOG(LogTemp, Log, TEXT("모닥불과 상호작용"));
		return;
	}


	UE_LOG(LogTemp, Warning, TEXT("프롭 데이터가 세팅되어 있지 않습니다."));

}

void AWorldProp::OnSelect_Implementation(bool bIsStarted)
{
	if (auto Widget = Cast<UInteractionWidget>(InteractionWidget->GetUserWidgetObject()))
	{
		Widget->UpdateInteraction(Data->PropType, Data->InteractionKey);
		InteractionWidget->SetVisibility(bIsStarted);

	}

}
