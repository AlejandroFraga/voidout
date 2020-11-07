
#include "ACTR_EnergyBarrier.h"
#include "Assets/ACTR_PowerGenerator.h"
#include "Assets/ACTR_SwitchableLight.h"
#include "Character/CHRT_Player.h"
#include "Components/ACMP_Health.h"
#include "Components/ACMP_LightStructureSubscription.h"
#include "Controller/CTRL_Player.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SystemsManager.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstance.h"
#include "TimerManager.h"

// Sets default values
AACTR_EnergyBarrier::AACTR_EnergyBarrier()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));

	m_collisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComp"));

	if (m_collisionComponent == nullptr)
	{
		USYST_LogManager::PrintError(this, "AACTR_EnergyBarrier.AACTR_EnergyBarrier: No collision component created");
		return;
	}
	
	m_collisionComponent->SetGenerateOverlapEvents(true);
	m_collisionComponent->SetupAttachment(RootComponent, "Collider");
	m_collisionComponent->BodyInstance.SetCollisionProfileName("CollisionEnergyBarrier");

	m_collisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	m_collisionComponent->CanCharacterStepUpOn = ECB_No;

	// Workaround. Prevents cached constructors to add delegates twice.
	m_collisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AACTR_EnergyBarrier::BeginOverlap);
	m_collisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &AACTR_EnergyBarrier::EndOverlap);

	// Bind overlap events
	m_collisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AACTR_EnergyBarrier::BeginOverlap);
	m_collisionComponent->OnComponentEndOverlap.AddDynamic(this, &AACTR_EnergyBarrier::EndOverlap);
}

// Called every frame
void AACTR_EnergyBarrier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isPlayerInside)
	{
		DamageInside(GetCHRTPlayer(), DeltaTime);
	}
}

void AACTR_EnergyBarrier::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> onBarriers = GetComponentsByTag(UStaticMeshComponent::StaticClass(), "OnBarrier");
	if (onBarriers.Num() > 0)
		m_onBarrier = Cast<UStaticMeshComponent>(onBarriers[0]);

	TArray<UActorComponent*> offBarriers = GetComponentsByTag(UStaticMeshComponent::StaticClass(), "OffBarrier");
	if (offBarriers.Num() > 0)
		m_offBarrier = Cast<UStaticMeshComponent>(offBarriers[0]);
}

void AACTR_EnergyBarrier::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsCHRTPlayer(OtherActor, OtherComp))
	{
		isPlayerInside = true;
		ACHRT_Player* player = GetCHRTPlayer();
		if (player != nullptr)
		{
			ForceFeedbackHit();

			SYST_SystemsManager::GetSoundEventManager()->playEnergyBarrierDamage(GetActorLocation());

			InitialDamage(player);
			InvertVelocity(player);
		}
	}
}

void AACTR_EnergyBarrier::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsCHRTPlayer(OtherActor, OtherComp))
		isPlayerInside = false;
}

void AACTR_EnergyBarrier::resetEnergyBarrierOnLoadGame()
{
	m_subscribedActors = 0;
	m_isActive = false;
}

bool AACTR_EnergyBarrier::IsCHRTPlayer(AActor* otherActor, UActorComponent* otherComp) const
{
	return otherActor != nullptr && Cast<ACHRT_Player>(otherActor) != nullptr
		&& otherComp != nullptr && Cast<UCapsuleComponent>(otherComp) != nullptr;
}

ACHRT_Player* AACTR_EnergyBarrier::GetCHRTPlayer() const
{
	if(UGameplayStatics::GetPlayerPawn(this, 0) != nullptr)
		return Cast<ACHRT_Player>(UGameplayStatics::GetPlayerPawn(this, 0));
	return nullptr;
}

void AACTR_EnergyBarrier::InitialDamage(ACHRT_Player* player) const
{
	if(player != nullptr)
		player->TakeDamage(repulsionDamage);
}

void AACTR_EnergyBarrier::DamageInside(ACHRT_Player* player, float deltaTime) const
{
	if (player != nullptr)
		player->TakeDamage(repulsionDamage * insideDamageMultiplier * deltaTime);
}

void AACTR_EnergyBarrier::InvertVelocity(ACHRT_Player* player)
{
	if (player == nullptr)
		return;

	const FVector playerVelocity = player->GetVelocity();
	const FVector2D velocity2DInverted = UKismetMathLibrary::GetRotated2D(FVector2D(playerVelocity.X, playerVelocity.Y), 180.f);

	if (player->GetMovementComponent() == nullptr)
		return;

	player->GetMovementComponent()->Velocity = FVector(velocity2DInverted.X, velocity2DInverted.Y, 0.f);
}

void AACTR_EnergyBarrier::SubscribeToWall()
{
	m_subscribedActors++;

	if (m_subscribedActors >= 1)
		LockWall();
}

void AACTR_EnergyBarrier::UnsubscribeToWall()
{
	m_subscribedActors--;

	if (m_subscribedActors <= 0)
	{
		if (GetWorld() != nullptr)
		{
			FTimerHandle timerHandler;
			GetWorld()->GetTimerManager().SetTimer(timerHandler, this, &AACTR_EnergyBarrier::UnLockWall, timeToUnlock, false);
		}
		else
			UnLockWall();
	}
}

void AACTR_EnergyBarrier::UnLockWall()
{
	ChangeWallStatus(false);
	SYST_SystemsManager::GetSoundEventManager()->playEnergyBarrierOff(GetActorLocation());

	if (m_isForStopMusic) {
		SYST_SystemsManager::GetSoundEventManager()->stopFightMusic();
	}

}

void AACTR_EnergyBarrier::LockWall()
{
	ChangeWallStatus(true);
	SYST_SystemsManager::GetSoundEventManager()->playEnergyBarrierOn(GetActorLocation());
}

void AACTR_EnergyBarrier::LockWallOnFinishLevel()
{
	ChangeWallStatus(false);
	if (m_onBarrier == nullptr)
		return;

	m_onBarrier->SetMaterial(1, m_energyBarrierLockedMaterial);
	if(m_onBarrier->GetMaterial(2))
		m_onBarrier->SetMaterial(2,	m_energyBarrierLockedMaterial);
}

void AACTR_EnergyBarrier::ChangeWallStatus(bool status)
{
	m_isActive = status;
	const auto newCollisionEnabled = status ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision;

	if (m_collisionComponent == nullptr)
		return;

	m_collisionComponent->SetCollisionEnabled(newCollisionEnabled);

	if (m_onBarrier != nullptr)
	{
		m_onBarrier->SetVisibility(status);
		m_onBarrier->SetCollisionEnabled(newCollisionEnabled);
	}

	if (m_offBarrier != nullptr)
	{
		m_offBarrier->SetVisibility(!status);
		m_offBarrier->SetCollisionEnabled(newCollisionEnabled);
	}

	TArray<UStaticMeshComponent*> meshes;
	GetComponents<UStaticMeshComponent>(meshes);
}
