
#include "AICT_Soldier.h"
#include "AIController/BLKB_Global.h"
#include "Assets/ACTR_SoldierBullet.h"
#include "Assets/ACTR_EnergyTower.h"
#include "Assets/ACTR_EnergyBarrier.h"
#include "Character/CHRT_Enemy.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SoundEventManager.h"
#include "System/SYST_SystemsManager.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"

void AAICT_Soldier::BeginPlay()
{
	Super::BeginPlay();

	if(BrainComponent != nullptr)
	{
		if (m_blackboardComponent == nullptr)
		{
			m_blackboardComponent = BrainComponent->GetBlackboardComponent();
		}
		else
		{
			m_blackboardComponent->SetValueAsBool("CheckForTower", true);
			m_blackboardComponent->SetValueAsFloat("TimeBetweenPatrols", m_timeBetweenPatrols);
		}
	}

	auto* pawn = GetPawn();
	if (Cast<ACHRT_Enemy>(pawn) != nullptr)
		m_blackboardComponent->SetValueAsBool("AbleToShoot", Cast<ACHRT_Enemy>(pawn)->m_dontShootThr == 1.0 ? false : true);

	if (m_perceptionComponent == nullptr && GetComponentByClass(UAIPerceptionComponent::StaticClass()) != nullptr)
	{
		m_perceptionComponent = Cast<UAIPerceptionComponent>(GetComponentByClass(UAIPerceptionComponent::StaticClass()));
	}
	else
		m_perceptionComponent->ForgetActor(BLKB_Global::getPlayerActor());

}

void AAICT_Soldier::InitBlackboardComponent()
{
	// If the blackboardComponent is not set, get it from the pawn
	if (m_blackboardComponent == nullptr && BrainComponent != nullptr)
	{
		m_blackboardComponent = BrainComponent->GetBlackboardComponent();
	}
	//return m_blackboardComponent;
}

void AAICT_Soldier::InitPerceptionComponent()
{
	// If the blackboardComponent is not set, get it from the pawn
	if (m_perceptionComponent == nullptr && GetComponentByClass(UAIPerceptionComponent::StaticClass()) != nullptr)
	{
		m_perceptionComponent = Cast<UAIPerceptionComponent>(GetComponentByClass(UAIPerceptionComponent::StaticClass()));
	}
	//return m_perceptionComponent;
}

// Shoot action.
void AAICT_Soldier::Shoot()
{
	Super::Shoot();

	m_state = 1;

	GoTo();
	const bool isPlayerNear = runAwayPlayer();
	m_blackboardComponent->SetValueAsBool("isPlayerNear", isPlayerNear);
}

//Set Enemy velocity
void AAICT_Soldier::GoTo()
{
	if (GetPawn() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Soldier.GoTo: No Pawn");
		return;
	}

	ClearFocus(EAIFocusPriority::Gameplay);

	float speed = m_enemyVelocity;

	switch (m_state) {
	case 0:
		if (m_patrolVelocity > 0.0f)
			speed = m_patrolVelocity;
		break;

	case 1:
		if (m_attackVelocity > 0.0f)
			speed = m_attackVelocity;
		break;
	default:
		speed = m_enemyVelocity;
	}

	UCharacterMovementComponent* movementComponent = GetPawn()->FindComponentByClass<UCharacterMovementComponent>();
	if(movementComponent != nullptr)
		movementComponent->MaxWalkSpeed = speed;

}

//Check if player is near to attack
bool AAICT_Soldier::CheckNearbyPlayer()
{
	const auto answer = Super::CheckNearbyPlayer();
	const auto previousCondition = m_blackboardComponent->GetValueAsBool("Shoot");

	if (GetPawn() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Soldier.GoTo: No Pawn");
		return false;
	}

	const float dist = FVector::DistXY(GetPawn()->GetActorLocation(), BLKB_Global::getPlayerLocation());
	if(dist > m_maxPlayerDistance)
	{
		m_perceptionComponent->ForgetActor(BLKB_Global::getPlayerActor());
		m_blackboardComponent->SetValueAsBool("isPlayerNear", false);
		m_blackboardComponent->SetValueAsBool("Shoot", false);
	}

	//Si se quiere que se mueva constantemente por el area de ataque, eliminar este if
	if (previousCondition != answer) {
		m_blackboardComponent->SetValueAsBool("CheckAttackPosition", answer);
	}
	return answer;
}

//Patrol inside tower area
void AAICT_Soldier::getNewPosition() 
{
	ClearFocus(EAIFocusPriority::Gameplay);
	
	m_state = 0;

	GoTo();

	FVector newPos{ FVector::ZeroVector };
	if (m_curTower == nullptr)
	{
		getNewTower();
	}
	// Check that the tower was overwritten
	if (m_curTower == nullptr)
		return;

	AACTR_EnergyTower* lightComponent = Cast<AACTR_EnergyTower>(m_curTower);

	if (lightComponent == nullptr)
		return;

	const float r1 = lightComponent->getlightArea(); //Radio de luz

	float x = 0.0f;
	float y = 0.0f;

	if (GetPawn() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Soldier.getNewPosition: No Pawn");
		return;
	}

	const auto myPos = GetPawn()->GetActorLocation();
	newPos = ACHRT_Enemy::GetRandomPointInCircle({ m_curTower->GetActorLocation().X, m_curTower->GetActorLocation().Y, myPos.Z }, r1);
	m_blackboardComponent->SetValueAsVector("NextTower", newPos);

	m_state = 1;
}

//Run away from player
bool AAICT_Soldier::runAwayPlayer()
{
	if (GetPawn() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Soldier.GoTo: No Pawn");
		return false;
	}

	const FVector pawnPos = GetPawn()->GetActorLocation();

	const float distance = GetCharacterDistance();
	if (abs(distance) > m_maxRunDistance)
	{
		return false;
	}

	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (playerPawn == nullptr)
		return false;
	
	const FVector pos = pawnPos - playerPawn->GetActorLocation();
	m_blackboardComponent->SetValueAsVector("NextTower", pawnPos + pos * m_movementFactor);
	return true;
}

//Spawn the bullet old version
void AAICT_Soldier::spawnBullet()
{
	APawn* pawn = GetPawn();
	if (pawn == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Soldier.GoTo: No Pawn");
		return;
	}

	// Get Location
	const auto actorLocation = pawn->GetActorLocation();

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	//UBlueprint* GeneratedBP = Cast<UBlueprint>(m_soldierBullet);
	if (!m_shootingProjectile)
	{
		USYST_LogManager::PrintError(this, "AAICT_Soldier::spawnBullet no shooting projectile");
		return;
	}

	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (playerPawn == nullptr)
		return;

	const auto spawnRot = UKismetMathLibrary::FindLookAtRotation(actorLocation, playerPawn->GetActorLocation());
	pawn->FaceRotation(spawnRot);
	const FVector offset = pawn->GetActorForwardVector() * 10;
	GetWorld()->SpawnActor<AActor>(m_shootingProjectile, actorLocation + offset, spawnRot, ActorSpawnParams);

	SYST_SystemsManager::GetSoundEventManager()->playLightTrooperShoot(actorLocation);
}

//Get Nearest tower. TODO
void AAICT_Soldier::getNewTower()
{
	initTowersArray();
}

//It's used to initialize towers list
void AAICT_Soldier::initTowersArray()
{
	APawn* pawn = GetPawn();
	if (pawn == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Soldier.GoTo: No Pawn");
		return;
	}

	//FIXME a lo mejor directamente hacerlo by class y así no depender de las tags?
	TArray<AActor*> towers;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "TowerLight", towers);

	if (towers.Num() <= 0)
		return;

	const auto spawnLoc = pawn->GetActorLocation(); // Get Location
	float towerDistance = FVector::DistSquared(spawnLoc, towers[0]->GetActorLocation());
	m_curTower = towers[0];

	for (auto* tower : towers)
	{
		if (tower == nullptr)
			continue;

		const float towerDistanceAux = FVector::DistSquared(pawn->GetActorLocation(), tower->GetActorLocation());

		if (towerDistanceAux < towerDistance)
		{
			m_curTower = tower;
			towerDistance = towerDistanceAux;
		}
	}
	
	m_blackboardComponent->SetValueAsObject("CurrentTower", m_curTower);
	m_blackboardComponent->SetValueAsVector("NextTower", m_curTower->GetActorLocation());
	m_blackboardComponent->SetValueAsBool("CheckForTower", false);
}

// Used to generate random number in a range
float AAICT_Soldier::RandomFloat(float a, float b)
{
	const float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	const float diff = b - a;
	const float r = random * diff;
	return a + r;
}

void AAICT_Soldier::suscribeToWall(APawn* wall)
{
	if (wall == nullptr)
		return;

	USYST_LogManager::PrintString(this, "DESDE ENEMY: SUSCRIBIENDO", false);

	if (wall != nullptr && wall->GetParentActor() != nullptr)
	{
		Cast<AACTR_EnergyBarrier>(wall->GetParentActor()->GetClass())->SubscribeToWall();
		USYST_LogManager::PrintString(this, "DESDE ENEMY: ENVIADO", false);
	}

}

//Set the time between shoot for this actor
void AAICT_Soldier::SetTimeBetweenShoot(float time)
{
	USYST_LogManager::PrintString(this, "SETEANDO TIEMPO DISPARO");
	if (time < 0.0f)
		m_blackboardComponent->SetValueAsFloat("TimeBetweenShoots", m_timeBetweenShoots);
	else
		m_blackboardComponent->SetValueAsFloat("TimeBetweenShoots", time);

}

/* Apply trigonometry to get the nearest point to the player inside the area.*/
void AAICT_Soldier::GetAttackAreaPoint()
{
	if (m_curTower == nullptr)
		return;

	const FVector towerPosition = m_curTower->GetActorLocation();

	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (playerPawn == nullptr)
		return;

	const FVector playerPosition = playerPawn->GetActorLocation();

	float distancePlayerTower = 0.0f;

	const float distX = powf(playerPosition.X - towerPosition.X, 2);
	const float distY = powf(playerPosition.Y - towerPosition.Y, 2);
	distancePlayerTower = sqrtf(distX + distY);

	AACTR_EnergyTower* lightComponent = Cast<AACTR_EnergyTower>(m_curTower);
	if (lightComponent == nullptr || distancePlayerTower == 0)
		return;

	//Calculate y position.
	float pY = (towerPosition.Y - playerPosition.Y) * lightComponent->getSecureArea() / distancePlayerTower + towerPosition.Y;
	//Calculate x position
	float pX = (towerPosition.X - playerPosition.X) * lightComponent->getSecureArea() / distancePlayerTower + towerPosition.X;

	//Check if Y value is not the oposite
	if (!IsInRange(playerPosition.Y, towerPosition.Y, pY)) {
		pY = pY - (pY - towerPosition.Y) * 2;
	}
	else if (!IsInRange(towerPosition.Y, playerPosition.Y, pY)) {
		pY = pY + (pY - towerPosition.Y) * 2;
	}
	//Check if X value is not the oposite
	if (!IsInRange(playerPosition.X, towerPosition.X, pX)) {
		pX = pX - (pX - towerPosition.X) * 2;
	}
	else if (!IsInRange(towerPosition.X, playerPosition.X, pX)) {
		pX = pX + (pX - towerPosition.X) * 2;
	}

	if (GetPawn() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Soldier.GetAttackAreaPoint: No Pawn");
		return;
	}

	const FVector newPos = FVector(pX, pY, GetPawn()->GetActorLocation().Z);
	USYST_LogManager::PrintString(this, "Posicion Tower = " + FString::FromInt(towerPosition.X) + ", " + FString::FromInt(towerPosition.Y), false);
	USYST_LogManager::PrintString(this, "Posicion Ataque = " + FString::FromInt(pX) + ", " + FString::FromInt(pY), false);
	USYST_LogManager::PrintString(this, "Posicion Player = " + FString::FromInt(playerPosition.X) + ", " + FString::FromInt(playerPosition.Y), false);
	m_blackboardComponent->SetValueAsVector("NextTower", newPos);
	m_blackboardComponent->SetValueAsBool("CheckAttackPosition", false);
}

bool AAICT_Soldier::IsInRange(float min, float max, float value)
{
	return min <= value && value <= max;
}