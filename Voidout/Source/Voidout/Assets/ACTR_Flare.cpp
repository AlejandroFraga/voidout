
#include "ACTR_Flare.h"
#include "AIController/BLKB_Global.h"
#include "Character/CHRT_Player.h"
#include "Components/ACMP_Health.h"
#include "Controller/CTRL_Player.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SystemsManager.h"
#include "System/SYST_DelayManager.h"

#include "DrawDebugHelpers.h"
#include "TimerManager.h"

// Sets default values
AACTR_Flare::AACTR_Flare()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_collisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	m_collisionComp->InitCapsuleSize(0.5, 2);
	m_collisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	m_collisionComp->OnComponentBeginOverlap.AddDynamic(this, &AACTR_Flare::BeginOverlap);

	// Players can't walk on it.
	m_collisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	m_collisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root.
	RootComponent = m_collisionComp;
}

// Called every frame
void AACTR_Flare::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (hitted)
		return;

	//FIXME en lugar de por la Z, cuando colisione con algo, para que no atraviese nada
	if(GetActorLocation().Z <= -125.f)
	{
		OnHitted();
		return;
	}
	m_velocity = FVector(m_velocity.X, m_velocity.Y, m_velocity.Z - m_gravity * DeltaTime);
	SetActorLocation(GetActorLocation() + m_velocity * DeltaTime);
}

void AACTR_Flare::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (hitted)
		return;

	if (OtherActor == nullptr || OtherActor == this || OtherComp == nullptr
		//FIXME TowerSearcher??
		|| OtherComp->ComponentHasTag("TowerSearcher"))
		return;

	APawn* pawn = Cast<APawn>(OtherActor);
	if (pawn != nullptr)
	{
		auto* playerController = Cast<ACTRL_Player>(pawn->GetController());
		if (playerController != nullptr)
		{
			BLKB_Global::seenByFlare(true);
			playerController->CallTimer(m_timeToMarkPlayer);
			OnHitted();
		}
	}
	//Detecting enviroment elements. Need collider to enter here
	if (OtherActor->GetClass() == AStaticMeshActor::StaticClass()) {
		OnHitted();
	}
}

void AACTR_Flare::OnHitted()
{
	if (hitted)
		return;

	hitted = true;
	PlayHittedSound();
	PlayHittedNPS();

	FTimerHandle timerHandler;
	GetWorld()->GetTimerManager().SetTimer(timerHandler, this, &AACTR_Flare::Clean, 1.f, false);
}

void AACTR_Flare::Clean()
{
	Destroy();
}