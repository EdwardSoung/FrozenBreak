// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/WorldProp.h"
#include "GameSystem/Events/StatusEvents.h"

#include "Components/SceneComponent.h"
#include "CommonComponents/StatComponent.h"
#include "Components/WidgetComponent.h"

#include "Objects/PickupItem.h"

#include "GameSystem/EventSubSystem.h"
#include "GameSystem/ItemFactorySubSystem.h"

#include "UI/Prop/InteractionWidget.h"
#include "Data/PropData.h"
#include "Interface/Interactable.h"
#include "GameSystem/TimeOfDaySubSystem.h"

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
	UE_LOG(LogTemp, Log, TEXT("WorldProp : 인터페이스 받았음"));
	// 주석 == ToDo
	// 어느정도 작성되면 타입별로 분리해 함수로 뺄 예정
	if (Data)
	{
		if (Data->PropType == EPropType::Tree)
		{
			TreeAction();
			return;
		}
		if (Data->PropType == EPropType::Rock)
		{
			RockAction();
			return;
		}
		if (Data->PropType == EPropType::Bed)
		{
			// 잘 수 있는 시간인지
			IsBedTime();

			// 잘 수 있을때만
			if (bIsBedTime)
			{
				BedAction();
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("잘 수 있는 시간이 아닙니다."));
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
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("프롭 데이터가 세팅되어 있지 않습니다."));
	}
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

void AWorldProp::TreeAction()
{
	// 중복실행을 막아야 함
	UE_LOG(LogTemp, Log, TEXT("나무와 상호작용"));

	if (EventSystem)
	{
		// 플레이어 피로도 감소? 시키기
		EventSystem->Status.OnSetFatigue.Broadcast(FatigueCostPerWork);

		// 임시. StatComponent->CurrentHealth에 데미지 주기
		StatComponent->OnPropDamaged(25.f);

		UE_LOG(LogTemp, Log, TEXT("나무를 베었다. 나무 Durability : %.1f"), StatComponent->CurrentHealth);
	}

	// 나무의 Durability가 0 이하 일 때
	if (StatComponent->CurrentHealth <= 0)
	{

		if (UItemFactorySubSystem* Factory = GetGameInstance()
			? GetGameInstance()->GetSubsystem<UItemFactorySubSystem>()
			: nullptr)
		{
			const float ZGap = 75.f;
			for (int32 i = 0; i < Data->GenerateItemCount; ++i)
			{
				const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, ZGap * i);

				Factory->Spawn(Data->GenerateItemType, SpawnLocation, i);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ItemFactorySubSystem이 없다."));
		}

		SetActorHiddenInGame(true);
		SetLifeSpan(0.001f);
	}
}

void AWorldProp::RockAction()
{
	// Player : 채굴하는 애님
	// 중복실행을 막아야 함
	UE_LOG(LogTemp, Log, TEXT("바위와 상호작용"));

	if (EventSystem)
	{
		// 플레이어 피로도 감소? 시키기
		EventSystem->Status.OnSetFatigue.Broadcast(FatigueCostPerWork);

		// 임시. StatComponent->CurrentHealth에 데미지 주기
		StatComponent->OnPropDamaged(25.f);
		UE_LOG(LogTemp, Log, TEXT("돌을 찍었다. 돌 Durability : %.1f"), StatComponent->CurrentHealth);
	}

	if (StatComponent->CurrentHealth <= 0)
	{

		if (UItemFactorySubSystem* Factory = GetGameInstance()
			? GetGameInstance()->GetSubsystem<UItemFactorySubSystem>()
			: nullptr)
		{
			const FVector Range(50.f, 50.f, 150.f);
			for (int32 i = 0; i < Data->GenerateItemCount; ++i)
			{
				// 위치 랜덤 오프셋
				const FVector RandomOffset(
					FMath::FRandRange(-Range.X, Range.X),
					FMath::FRandRange(-Range.Y, Range.Y),
					FMath::FRandRange(-Range.Z, Range.Z));

				const FVector SpawnLocation = GetActorLocation() + RandomOffset;

				Factory->Spawn(Data->GenerateItemType, SpawnLocation, i);
			}

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ItemFactorySubSystem이 없다."));
		}

		SetActorHiddenInGame(true);
		SetLifeSpan(0.001f);
	}

}

void AWorldProp::BedAction()
{
	// Player : 침대에 눕는 애님은 없으니 위젯 애니메이션으로 검어졌다가 시간지나고 뭐 텍스트 띄우고...
	// 중복실행을 막아야 함
	if (EventSystem)
	{
		EventSystem->Status.OnSetFatigue.Broadcast(FatigueRecoveryAmount);
		EventSystem->Status.OnSetHunger.Broadcast(HungerReductionAmount);
		UE_LOG(LogTemp, Log, TEXT("BroadCast 보냄."));
	}
	GetWorld()->GetSubsystem<UTimeOfDaySubSystem>()->SkipTimeByHours(BedUsageHours);
}

void AWorldProp::IsBedTime()
{
	// 근데 이게 여기에 있어도 되는 것인가

	float TimeNormalized = GetWorld()->GetSubsystem<UTimeOfDaySubSystem>()->GetTimeNormalized();
	const int32 TotalMinutes = FMath::FloorToInt(TimeNormalized * 24.0f * 60.0f);
	const int32 Hour = (TotalMinutes / 60) % 24;

	// 24시간제 사용
	// 이 시간부터 잘 수 있다. (테스트용 임시값)
	const int32 BedTimeStart = 12;

	// 이 시간부터 잘 수 없다. (테스트용 임시값)
	const int32 BedTimeEnd = 13;

	// BedTimeStart와 BedTimeEnd 사이의 시간이여야만 잘 수 있다.
	bIsBedTime = (Hour >= BedTimeStart) && (Hour < BedTimeEnd);
}
