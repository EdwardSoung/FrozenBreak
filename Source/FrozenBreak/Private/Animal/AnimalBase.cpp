// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/AnimalBase.h"
#include "Animal/AI/AnimalAIControllerBase.h"
#include "GameSystem/ItemFactorySubSystem.h"
#include "Components/WidgetComponent.h"
#include "UI/Enemy/EnemyHealthBarWidget.h"

// Sets default values
AAnimalBase::AAnimalBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
	HealthBarWidgetComponent->SetRelativeLocation(FVector(0, 0, 100));
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void AAnimalBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	HealthBarWidgetComponent->InitWidget(); // 위젯 생성 보장(중요)
	HealthBar = Cast<UEnemyHealthBarWidget>(HealthBarWidgetComponent->GetUserWidgetObject());

	if (HealthBar)
	{
		HealthBar->SetHealthProgress(CurrentHealth / MaxHealth);
	}

	OnTakeAnyDamage.AddDynamic(this, &AAnimalBase::OnTakeDamage);
}

// Called to bind functionality to input
void AAnimalBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAnimalBase::OnTakeDamage(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType, 
	AController* InstigatedBy, 
	AActor* DamageCauser)
{
	PlayHit();
	
	CurrentHealth = FMath::Max(CurrentHealth - Damage, DefaultMinimum);
	if (HealthBar)
	{
		HealthBar->SetHealthProgress(CurrentHealth / MaxHealth);
		StartHealthBarTimer();
		if (CurrentHealth <= DefaultMinimum)
		{
			PlayDead();
		}
		if (CurrentHealth <= MaxHealth / 2)
		{
			IDamageable::Execute_OnHalfHealth(this);
		}
	}
}

void AAnimalBase::StartHealthBarTimer()
{
	HealthBarWidgetComponent->SetVisibility(true);

	GetWorldTimerManager().SetTimer(
		HealthBarTimerHandle,
		[this]() {
			HealthBarWidgetComponent->SetVisibility(false);
		},
		HealthBarVisibleTime,
		false
		);
}

void AAnimalBase::PlayHit()
{
	if (AnimalHitAnimation)
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
			{
				Anim->Montage_Play(AnimalHitAnimation);
			}
		}
	}
}

void AAnimalBase::PlayDead()
{
	IDamageable::Execute_OnDead(this);
	HealthBarWidgetComponent->SetVisibility(false);

	if (AAnimalAIControllerBase * MyController = Cast<AAnimalAIControllerBase>(GetController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("UnPossess %s"), *GetController()->GetName());
		MyController->UnPossess();
	}

	if (AnimalDeadAnimation)
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
			{
				Anim->Montage_Play(AnimalDeadAnimation);
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		DeadTimerHandle,
		this,
		&AAnimalBase::DestroyAnimal,
		2.5f,
		false
	);
}

void AAnimalBase::DestroyAnimal()
{
	if (UItemFactorySubSystem* ItemFactory = UItemFactorySubSystem::Get(this))
	{
		ItemFactory->Spawn(EItemType::Lether, GetActorLocation() + FVector(10, 10, 50), LeatherDropCount);
		for (int i = 0; i < RawMeatDropCount; i++)
		{
			ItemFactory->Spawn(EItemType::RawMeat, GetActorLocation() + FVector(-10, -10, 50), DefaultMinimum);
		}
	}
	Destroy();
}
