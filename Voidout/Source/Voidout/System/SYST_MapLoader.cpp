
#include "SYST_MapLoader.h"
#include "Assets/ACTR_EnergyBarrier.h"
#include "Character/CHRT_Player.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASYST_MapLoader::ASYST_MapLoader()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	m_overlapTrg = CreateDefaultSubobject<UBoxComponent>(TEXT("Overlap trigger"));

	RootComponent = m_overlapTrg;
}

// Begin Play
void ASYST_MapLoader::BeginPlay()
{
	// Add overplays.
	m_overlapTrg->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASYST_MapLoader::OverlapBegin);
	m_overlapTrg->OnComponentEndOverlap.AddUniqueDynamic(this, &ASYST_MapLoader::OverlapEnd);
}

// On overlap.
void ASYST_MapLoader::OverlapBegin(UPrimitiveComponent* ovelapedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIdx, bool fromSweep, const FHitResult& sweepRes)
{
	if (beginUsed || !IsPlayer(otherActor, otherComp))
		return;

	if (m_mapToLoad != "")
	{
		const FLatentActionInfo latInfo;
		UGameplayStatics::LoadStreamLevel(this, m_mapToLoad, true, false, latInfo);
	}

	beginUsed = true;
}

// End overlap.
void ASYST_MapLoader::OverlapEnd(UPrimitiveComponent* ovelapedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIdx)
{
	if (!beginUsed || endUsed || !IsPlayer(otherActor, otherComp))
		return;

	if (m_mapToUnload != "")
	{
		const FLatentActionInfo latInfo;
		UGameplayStatics::UnloadStreamLevel(this, m_mapToUnload, latInfo, false);
	}

	// Suscribe to a wall if exists.
	AACTR_EnergyBarrier* barrier = Cast<AACTR_EnergyBarrier>(m_energyBarrier);
	if (barrier != nullptr)
	{
		barrier->LockWallOnFinishLevel();
		barrier->SubscribeToWall();
	}

	ACHRT_Player* playerCHRT = Cast<ACHRT_Player>(otherActor);
	if (playerCHRT != nullptr)
		playerCHRT->SaveGame();

	endUsed = true;
}

bool ASYST_MapLoader::IsPlayer(AActor* otherActor, UPrimitiveComponent* otherComp)
{
	return otherActor != nullptr && Cast<ACHRT_Player>(otherActor) != nullptr
		&& otherComp != nullptr
		&& (Cast<USkeletalMeshComponent>(otherComp) != nullptr || Cast<UCapsuleComponent>(otherComp) != nullptr);
}
