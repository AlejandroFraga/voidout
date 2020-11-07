
#include "AICT_Minion.h"
#include "AIController/BLKB_Global.h"
#include "Assets/ACTR_SoldierBullet.h"
#include "Character/CHRT_Enemy.h"
#include "Components/ACMP_Health.h"
#include "System/SYST_LogManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "System/SYST_SystemsManager.h"

// Ctor.
AAICT_Minion::AAICT_Minion()
{
	static ConstructorHelpers::FClassFinder<AACTR_SoldierBullet> bulletOb(TEXT("/Game/Blueprints/Assets/ACTR_Bullet_BP"));
	m_shootingProjectile = bulletOb.Class;
}

// Begin Play.
void AAICT_Minion::BeginPlay()
{
	Super::BeginPlay();
	
	// Set TimeBetweenShoots
	if (!GetBlackboardComponent()) return;
	
	m_blackboardComponent->SetValueAsFloat("TimeBetweenShoots", m_timeBetweenShoots);
}

UBlackboardComponent* AAICT_Minion::GetBlackboardComponent()
{
	// If the blackboardComponent is not set, get it from the pawn
	if (m_blackboardComponent == nullptr && BrainComponent != nullptr)
	{
		m_blackboardComponent = BrainComponent->GetBlackboardComponent();
	}
	return m_blackboardComponent;
}

void AAICT_Minion::CheckPlayerHP() noexcept
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Minion.CheckPlayerHP: Nullptr BlackboardComponent");
		return;
	}

	const auto* playerActor = BLKB_Global::getPlayerActor(); // Get player actor.
	if (playerActor == nullptr)
		return;

	// Get HP component.
	const auto* genpCMP = playerActor->GetComponentByClass(UACMP_Health::StaticClass());
	if (genpCMP == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Minion.CheckPlayerHP: Player has no healthComp");
		return;
	}

	const auto* HpCMP = Cast<UACMP_Health>(genpCMP);
	if (HpCMP != nullptr)
		return;
	
	// Check if's 0.
	const auto playerHP = HpCMP->getHp();
	if (playerHP > 0)
		return;

	m_blackboardComponent->SetValueAsBool("canSeePlayer", false);
}

float AAICT_Minion::GetCharacterDistance() const
{
	APawn* pPlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (pPlayerPawn == nullptr || GetPawn() == nullptr)
		return 0.f;

	const FVector playerLoc = pPlayerPawn->GetActorLocation();
	const FVector pawnPos = GetPawn()->GetActorLocation();

	float euclideDistance = 0.0f;

	const float distX = powf(playerLoc.X - pawnPos.X, 2);
	const float distY = powf(playerLoc.Y - pawnPos.Y, 2);
	euclideDistance = sqrtf(distX + distY);

	return euclideDistance;

}

bool AAICT_Minion::CheckNearbyPlayer() const
{
	const float playerDistance = GetCharacterDistance();
	const bool near = playerDistance <= m_maxPlayerDistance;
	return near;
}

void AAICT_Minion::Move()
{
	CheckPlayerHP();
}

std::tuple<bool, float, float> AAICT_Minion::SolveQuadratic(float a, float b, float c) noexcept
{
	const auto discr = (b * b) - (4 * a * c);
	bool canBeSolved = true;
	
	if (discr < 0)
		canBeSolved = false;
	
	const auto numeratorSqrtResult = sqrt(discr);
	const auto denominatorResult = 2 * a;
	const auto addNegativeTo_b = -b;

	if (denominatorResult == 0)
		canBeSolved = false;

	auto addOp = (addNegativeTo_b + numeratorSqrtResult) / denominatorResult;
	auto subOp = (addNegativeTo_b - numeratorSqrtResult) / denominatorResult;
	
	return std::tuple<bool, float, float>{ canBeSolved, addOp, subOp};
}

void AAICT_Minion::Shoot()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Minion.CheckPlayerHP: Nullptr BlackboardComponent");
		return;
	}

	// Set target to follow.
	auto* parentPawn = GetPawn();
	if (parentPawn == nullptr)
		return;

	auto* parentEnemy = Cast<ACHRT_Enemy>(parentPawn);
	if (parentEnemy == nullptr)
		return;
	
	// Unshoot threshold
	const auto dontShootThr = parentEnemy->m_dontShootThr;
	m_blackboardComponent->SetValueAsBool("AbleToShoot", dontShootThr != 1.0);
	
	// Success shoot threshhold
	const auto successShootThr = parentEnemy->m_successShootThr;

	// Get shoot probability
	const float randShootOrNot = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	if(randShootOrNot <= dontShootThr)
		return;
	
	const auto spawnLoc = parentPawn->GetActorLocation() + parentPawn->GetActorForwardVector() * 10;

	// Get player.
	auto* playerActor = BLKB_Global::getPlayerActor();
	if (playerActor == nullptr)
		return;

	// Get player velocity.
	const auto playerVelocity = playerActor->GetVelocity();

	//FIXME Número mágico
	const auto bulletSpeed = 2000.f;

	const auto a = FVector::DotProduct(playerVelocity, playerVelocity) - (bulletSpeed * bulletSpeed);
	const auto b = 2 * FVector::DotProduct(playerActor->GetActorLocation() - spawnLoc, playerVelocity);
	const auto distance = parentPawn->GetActorLocation() - playerActor->GetActorLocation();
	const auto c = FVector::DotProduct(distance, distance);

	auto* world = GetWorld();
	if (world == nullptr)
		return;

	FActorSpawnParameters actParams;
	actParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const auto oldPlayerPosRotation = UKismetMathLibrary::FindLookAtRotation(spawnLoc, playerActor->GetActorLocation());
	
	auto quad = SolveQuadratic(a, b, c);
	
	// Unpredicted shoot.
	if(!std::get<0>(quad)) 
	{
		// If cannot be solved, shoot at curret position.
		world->SpawnActor<AACTR_SoldierBullet>(m_shootingProjectile, spawnLoc, oldPlayerPosRotation, actParams);
	}

	const auto add = std::get<1>(quad);
	const auto sub = std::get<2>(quad);
	
	const auto mindist  = add < sub && sub > 0 || add < 0 ? sub : add;
	const auto newPoint = playerActor->GetActorLocation() + playerVelocity * mindist;

	SYST_SystemsManager::GetSoundEventManager()->playLightTrooperShoot(parentPawn->GetActorLocation());

	// Get shoot probability
	const float rLocation = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	// If lowerst than threshold, then shoot to predicted.
	const auto finalRotation = rLocation <= successShootThr ? UKismetMathLibrary::FindLookAtRotation(spawnLoc, newPoint) : oldPlayerPosRotation; 
	world->SpawnActor<AACTR_SoldierBullet>(m_shootingProjectile, spawnLoc, finalRotation, actParams);
	const float dist = FVector::DistXY(parentPawn->GetActorLocation(), BLKB_Global::getPlayerLocation());
	if (dist > m_maxPlayerDistance)
	{
		UAIPerceptionComponent* componentPerception = Cast<UAIPerceptionComponent>(GetComponentByClass(UAIPerceptionComponent::StaticClass()));
		if (componentPerception != nullptr)
		{
			componentPerception->ForgetActor(BLKB_Global::getPlayerActor());
			m_blackboardComponent->SetValueAsBool("CanSeePlayer", false);
		}
	}
}

void AAICT_Minion::Focus()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Minion.CheckPlayerHP: Nullptr BlackboardComponent");
		return;
	}

	CheckPlayerHP();
	SetFocus(BLKB_Global::getPlayerActor());
}

void AAICT_Minion::ResetFocus()
{
	SetFocus(nullptr);
}