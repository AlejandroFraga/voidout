
#include "AICT_Commander.h"
#include "AIController/AICT_Minion.h"
#include "Character/CHRT_Commander.h"
#include "Character/CHRT_Enemy.h"
#include "Components/ACMP_Health.h"
#include "System/SYST_LogManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "BLKB_Global.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/SYST_SystemsManager.h"

AAICT_Commander::AAICT_Commander()
{
	// The actor can tick
	PrimaryActorTick.bCanEverTick = true;
}

// Set the target path.
void AAICT_Commander::SetTargetPath()
{
	auto* owner = GetPawn();
	if (!owner) return;
	auto* comm = Cast<ACHRT_Commander>(owner);
	m_path = comm->m_path;
	m_pathSize = m_path.Num();
}

// Get the next target point.
void AAICT_Commander::GetNextPoint()
{
	if (m_pathSize == 0) SetTargetPath();
	if (m_pathSize == 0) return;

	// Update increment.
	if (m_nextPathPoint == 0)					m_pathIncrement = 1;
	else if (m_nextPathPoint == m_pathSize - 1) m_pathIncrement = -1;

	m_nextPathPoint += m_pathIncrement; // Set new index.
	const FVector nextPoint{ m_path[m_nextPathPoint]->GetActorLocation() };
		
	GetBlackboardComponent()->SetValueAsVector("m_nextTargetPoint", nextPoint);
}

// Chase the player.
void AAICT_Commander::ChasePlayer()
{
	const auto plyLoc = BLKB_Global::getPlayerLocation();
	const FVector nextPoint{ plyLoc };
	GetBlackboardComponent()->SetValueAsVector("m_nextTargetPoint", nextPoint);

	auto* pawn = GetPawn();
	if (!pawn) return;
	const auto distToPlayer = FVector::Dist(pawn->GetActorLocation(), plyLoc);
	
	auto* comm = Cast<ACHRT_Commander>(pawn);
	if(distToPlayer < comm->m_distToHit) // If commader is near, runs.
	{
		comm->GetCharacterMovement()->MaxWalkSpeed = comm->m_runningSpeed;
	}
	else
	{
		comm->GetCharacterMovement()->MaxWalkSpeed = comm->m_walkingSpeed;
	}
	GetBlackboardComponent()->SetValueAsBool("m_canHitPlayer", distToPlayer < comm->m_distToHit); // If closer than m_dist, set to true.
}

// Bassic attack.
void AAICT_Commander::Attack() const noexcept
{
	auto* commander = Cast<ACHRT_Commander>(GetPawn()); // Gets's the pawn.
	if (commander == nullptr)
		return;

	commander->Attack(); // Call to character attack.
	//SYST_SystemsManager::GetSoundEventManager()->playCommanderDamage(commander->GetActorLocation());
}

void AAICT_Commander::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto* pawn = GetPawn();
	if (!pawn) return;
	auto* comm = Cast<ACHRT_Commander>(pawn);

	if (comm->m_health == nullptr || comm->m_health->m_currentHp == comm->m_maxHp)
		return;
	
	auto& sumSgs = comm->m_sumSegs;
	sumSgs += DeltaTime;
	
	if(sumSgs >= comm->m_segsToHeal)
	{
		comm->m_health->m_currentHp = comm->m_maxHp;
		sumSgs -= comm->m_segsToHeal;
	}
}

void AAICT_Commander::MoveMinions()
{
	auto* pawn = GetPawn();
	if (!pawn) return;

	const auto nextPos{ GetBlackboardComponent()->GetValueAsVector("m_nextTargetPoint") }; // Next position that we are going to move.

	auto* comm = Cast<ACHRT_Commander>(pawn);
	if (!comm) return;

	for(auto* c : comm->m_children)
	{
		if (!c) continue;

		auto* health = c->FindComponentByClass<UACMP_Health>();
		if (!health || health->m_dying) continue;

		auto* ctrl = Cast<AAIController>(c->GetController());
		if (!ctrl) continue;
		
		ctrl->MoveToLocation(ACHRT_Enemy::GetRandomPointInCircle(nextPos, comm->m_spawnChildrenRadius), comm->m_spawnChildrenRadius); // Get a random point in circle, and move to that point.
	}
}