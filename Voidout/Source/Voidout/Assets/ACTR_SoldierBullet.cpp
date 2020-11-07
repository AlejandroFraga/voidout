
#include "ACTR_SoldierBullet.h"
#include "AIController/AICT_Soldier.h"
#include "Controller/CTRL_Player.h"
#include "Character/CHRT_Enemy.h"
#include "Character/CHRT_Player.h"
#include "System/SYST_SoundEventManager.h"
#include "System/SYST_SystemsManager.h"

#include "Components/CapsuleComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"


// Sets default values
AACTR_SoldierBullet::AACTR_SoldierBullet()
{
	PrimaryActorTick.bCanEverTick = false;

	m_collisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));

	if (m_collisionComp == nullptr)
		return;

	m_collisionComp->InitCapsuleSize(0.5, 2);
	m_collisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	m_collisionComp->OnComponentBeginOverlap.AddDynamic(this, &AACTR_SoldierBullet::BeginOverlap);

	// Players can't walk on it.
	m_collisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	m_collisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root.
	RootComponent = m_collisionComp;

	// Projectile
	m_projMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));

	if (m_projMovement == nullptr)
		return;

	m_projMovement->InitialSpeed = m_speed;
	m_projMovement->bShouldBounce = false;
	m_projMovement->MaxSpeed = m_speed;
	m_projMovement->UpdatedComponent = m_collisionComp;
	m_projMovement->ProjectileGravityScale = 0; // Gravity = 0;

	// Particles
	m_particles = CreateDefaultSubobject<UParticleSystem>(TEXT("ParticlesComp"));
}

void AACTR_SoldierBullet::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || OtherActor == this || OtherComp->ComponentHasTag("TowerSearcher"))
		return;

	ACHRT_Player* player = Cast<ACHRT_Player>(OtherActor);
	if (player != nullptr)
	{
		player->DelayDamage(m_damage);
		SYST_SystemsManager::GetSoundEventManager()->playPlayerDamage(player->GetActorLocation());
		//Destroy();
	}

	ACHRT_Enemy* enemy = Cast<ACHRT_Enemy>(OtherActor);
	if (enemy == nullptr && OtherComp->ComponentHasTag("LightStructureMesh") || OtherActor->GetClass() == AStaticMeshActor::StaticClass())
		Destroy();
}
