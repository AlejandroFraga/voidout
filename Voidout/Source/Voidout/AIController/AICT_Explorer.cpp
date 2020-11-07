
#include "AICT_Explorer.h"
#include "AIController/BLKB_Global.h"
#include "Assets/ACTR_EnergyTorche.h"
#include "Assets/ACTR_EnergyTower.h"
#include "Assets/ACTR_Flare.h"
#include "Assets/TGPT_Explorer.h"
#include "Character/CHRT_Explorer.h"
#include "Components/ACMP_Health.h"
#include "System/SYST_LogManager.h"

#include <math.h>
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Called when the game starts or when spawned
void AAICT_Explorer::BeginPlay()
{
	Super::BeginPlay();

	// Check if the explorer has BlackboardComponent
	if (GetBlackboardComponent() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.BeginPlay: No BlackboardComponent");
		return;
	}

	// Set blackboard initial values
	m_blackboardComponent->SetValueAsInt("IndexDestination", 1);
	m_blackboardComponent->SetValueAsInt("TargetPointIndex", 0);
	m_blackboardComponent->SetValueAsBool("IsRunningFromPlayer", false);
	m_blackboardComponent->SetValueAsBool("SeePlayer", false);
	m_blackboardComponent->SetValueAsBool("isOnPositionToShoot", false);
	m_blackboardComponent->SetValueAsBool("HasOtherTower", true);
	m_blackboardComponent->SetValueAsFloat("WaitTime", -1.0);
	m_blackboardComponent->SetValueAsFloat("TimeBetweenShoot", -1.0);
	
	if (UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		m_velocityHistoryBuffer[0] = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetVelocity();;
}

UBlackboardComponent* AAICT_Explorer::GetBlackboardComponent()
{
	// If the blackboardComponent is not set, get it from the pawn
	if (m_blackboardComponent == nullptr && BrainComponent != nullptr)
	{
		m_blackboardComponent = BrainComponent->GetBlackboardComponent();
	}
	return m_blackboardComponent;
}

ACHRT_Explorer* AAICT_Explorer::GetExplorerCharacter()
{
	// If the explorerCharacter is not set, get it from the pawn
	if (m_explorerCharacter == nullptr)
	{
		m_explorerCharacter = Cast<ACHRT_Explorer>(GetPawn());
	}
	return m_explorerCharacter;
}

void AAICT_Explorer::CheckNearbyEnemy()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr || GetExplorerCharacter() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckNearbyEnemy: Nullptr BlackboardComponent or ExplorerCharacter");
		return;
	}

	// Check for towers
	CheckTowers();
	
	// Check if the explorer is running from the player
	if (m_blackboardComponent->GetValueAsBool("IsRunningFromPlayer"))
		return;

	//if velocity buffer is intialized add the player velocity
	if(m_velocityHistoryBuffer.Capacity() > 0)
	{
		int32 nextIndex = m_velocityHistoryBuffer.GetNextIndex(indexBuffer);
		if (UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
			m_velocityHistoryBuffer[nextIndex] = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetVelocity();
		if(nextIndex < 50)
		{
			indexBuffer = nextIndex;
		}
		else
		{
			indexBuffer = 0;
		}
	}

	// If the wait time is not set, set the current value
	if (m_blackboardComponent->GetValueAsFloat("WaitTime") == -1.0)
		m_blackboardComponent->SetValueAsFloat("WaitTime", m_explorerCharacter->m_waitForTocrh);

	// If the wait time is not set, set the current value
	if (m_blackboardComponent->GetValueAsFloat("TimeBetweenShoot") == -1.0)
		m_blackboardComponent->SetValueAsFloat("TimeBetweenShoot", m_explorerCharacter->m_timeBetweenShoot);

	//Check if the player is see and if there's another enemy nearby
	CheckSeePlayer();
	CheckOtherEnemyNearby();
}

void AAICT_Explorer::CheckSeePlayer()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr || GetExplorerCharacter() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckSeePlayer: Nullptr BlackboardComponent or ExplorerCharacter");
		return;
	}

	//FIXME este codigo necesita explicación, es completamente opaco
	const float dist = FVector::DistXY(m_explorerCharacter->GetActorLocation(), BLKB_Global::getPlayerLocation());
	const FVector vec = BLKB_Global::getPlayerLocation() - m_explorerCharacter->GetActorLocation();
	const FVector fwd = m_explorerCharacter->GetActorForwardVector();
	const float dotProd = FVector::DotProduct(vec, fwd);
	const float magnitudes = vec.Size() * fwd.Size();

	if (magnitudes == 0)
		return;

	const float angle = acos(dotProd / magnitudes);

	//Si se aleja el player
	if (dist >= m_explorerCharacter->m_visionConeLength)
	{
		m_blackboardComponent->SetValueAsBool("SeePlayer", false);
		m_blackboardComponent->SetValueAsBool("PlayerNear", false);
	}
	
	//Si esta de frente el sensing lo maneja
	if(angle <= 1.0f)
	{
		return;
	}
	
	// If the player is near
	if (dist <= m_explorerCharacter->m_visionConeLength && angle > 1.0f)
	{
		//GWorld->LineTraceTestByChannel(m_explorerCharacter->GetActorLocation(), BLKB_Global::getPlayerLocation(), OcclusionDetails.OcclusionTraceChannel, Params);
		FCollisionQueryParams traceParams;
		traceParams.AddIgnoredActor(this);
		FHitResult hitResult;

		//check if is something betwen explorer and player
		if (GetWorld()->LineTraceSingleByChannel(hitResult, m_explorerCharacter->GetActorLocation(), BLKB_Global::getPlayerLocation(), ECC_Visibility, traceParams)) {
			return;
		}
		// Set the value in the blackboard, stop running from the player
		m_blackboardComponent->SetValueAsBool("PlayerNear", true);
		m_blackboardComponent->SetValueAsBool("SeePlayer", true);
		RunAwayFromPlayer(false);
	}
	// Otherwise
	else
	{
		// Set the value in the blackboard, change movement to walk speed
		m_blackboardComponent->SetValueAsBool("PlayerNear", false);
		m_explorerCharacter->GetCharacterMovement()->MaxWalkSpeed = m_explorerCharacter->m_walkSpeed;
	}
}

void AAICT_Explorer::CheckOtherEnemyNearby()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr || GetExplorerCharacter() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckOtherEnemyNearby: Nullptr BlackboardComponent or ExplorerCharacter");
		return;
	}

	float distanceEnemy{ 0.0f };
	int numEnemies{ 0 };

	//FIXME Todos los actores del mundo? No se puede mejorar
	// We check all the enemies in the world to count
	for (TActorIterator<ACHRT_Enemy> it(GetWorld()); it; ++it)
	{
		// Check that the enemy is valid and not an explorer
		if (*it == nullptr || Cast<ACHRT_Explorer>(*it) == nullptr)
			continue;

		// If the enemy is inside the vision cone, it counts
		distanceEnemy = FVector::DistXY(m_explorerCharacter->GetActorLocation(), it->GetActorLocation());
		if (distanceEnemy <= m_explorerCharacter->m_visionConeLength)
		{
			numEnemies++;
			break;
		}
	}
	// Set the value
	m_blackboardComponent->SetValueAsInt("OtherEnemiesCount", numEnemies);
}

void AAICT_Explorer::CheckTowers()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr || GetExplorerCharacter() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckTowers: Nullptr BlackboardComponent or ExplorerCharacter");
		return;
	}

	TArray<ATGPT_Explorer*> explorerPath = m_explorerCharacter->m_path;

	// Has no target points
	if (explorerPath.Num() <= 0)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckTowers: No target points");
		return;
	}

	// Has 1 target point
	if (explorerPath.Num() <= 1 && m_blackboardComponent->GetValueAsBool("HasOtherTower"))
	{
		m_blackboardComponent->SetValueAsBool("HasOtherTower", false);
		return;
	}

	// Get the last target point
	ATGPT_Explorer* lastTargetPoint = explorerPath.Last();

	// Check if it's valid
	if (lastTargetPoint == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckTowers: Has no last target point");
		return;
	}

	// Get the last target point energy tower
	AACTR_EnergyTower* lastEnergyTower = lastTargetPoint->m_energyTower;
	if (lastEnergyTower != nullptr)
	{
		// Check if it's valid
		if (lastEnergyTower == nullptr)
		{
			USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckTowers: Last target point has no energy tower");
			return;
		}

		// If it's off, update the blackboard info
		if (!lastEnergyTower->isLightOnPermanent())
			m_blackboardComponent->SetValueAsBool("HasOtherTower", false);
	}

	// Get the first target point
	ATGPT_Explorer* firstTargetPoint = explorerPath[0];

	// Check if it's valid
	if (firstTargetPoint == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckTowers: Has no first target point");
		return;
	}

	// Get the first target point energy tower
	AACTR_EnergyTower* firstEnergyTower = firstTargetPoint->m_energyTower;
	if (firstEnergyTower != nullptr)
	{
		// Check if it's valid
		if (firstEnergyTower == nullptr)
		{
			USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckTowers: First target point has no energy tower");
			return;
		}

		// If it's off
		if (!firstEnergyTower->isLightOnPermanent())
		{
			if (m_explorerCharacter)
				m_explorerCharacter->Kill();
		}
	}
}

bool AAICT_Explorer::GoToInitialTower()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr || GetExplorerCharacter() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.GoToInitialTower: Nullptr BlackboardComponent or ExplorerCharacter");
		return false;
	}

	TArray<ATGPT_Explorer*> explorerPath = m_explorerCharacter->m_path;

	int32 targetPoinIndex = m_blackboardComponent->GetValueAsInt("TargetPointIndex");

	if(targetPoinIndex == 1)
	{
		FVector newPos = FVector::ZeroVector;

		if (explorerPath.Num() <= 0
			&& explorerPath[0] == nullptr)
		{
			USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckTowers: No target points or first is nullptr");
			return false;
		}

		const AACTR_EnergyTower* energyTower = explorerPath[0]->m_energyTower;
		
		if (energyTower == nullptr)
		{
			USYST_LogManager::PrintError(this, "AAICT_Explorer.CheckTowers: First target point has no energy tower");
			return false;
		}

		//PRUEBA PARA UN SOLO RADIO
		float r1 = energyTower->getlightArea(); //Radio de luz
		if (r1 == 0)
		{
			r1 = 500;
		}
		float x = 0.0f;
		float y = 0.0f;
		
		do  {
			x = RandomFloat(energyTower->GetActorLocation().X - r1, energyTower->GetActorLocation().X + r1);
			y = RandomFloat(energyTower->GetActorLocation().Y - r1, energyTower->GetActorLocation().Y + r1);
			newPos = FVector(x, y, 0);
		} while (FVector::Dist(energyTower->GetActorLocation(), newPos) > r1);
		
		m_blackboardComponent->SetValueAsInt("TargetPointIndex", 0);
		m_blackboardComponent->SetValueAsVector("TargetPointPosition", newPos);
		return true;
	}
	else if(targetPoinIndex <= 0)
	{
		m_blackboardComponent->SetValueAsBool("isOnPositionToShoot", true);
		return true;
	}
	else
	{
		targetPoinIndex = targetPoinIndex - 1;
	}

	const auto* nextPoint = explorerPath[targetPoinIndex];
	if (!nextPoint) return false;
	m_blackboardComponent->SetValueAsInt("TargetPointIndex", targetPoinIndex);
	m_blackboardComponent->SetValueAsVector("TargetPointPosition", nextPoint->GetActorLocation());
	return false;
}

// Used to generate random number in a range
float AAICT_Explorer::RandomFloat(float a, float b) const{
	const float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	const float diff = b - a;
	const float r = random * diff;
	return a + r;
}

void AAICT_Explorer::GetNextTargetPoint()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr || GetExplorerCharacter() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.GetNextTargetPoint: Nullptr BlackboardComponent or ExplorerCharacter");
		return;
	}

	TArray<ATGPT_Explorer*> explorerPath = m_explorerCharacter->m_path;

	int32 targetPoinIndex = m_blackboardComponent->GetValueAsInt("TargetPointIndex");

	if (!m_blackboardComponent->GetValueAsBool("HasOtherTower")) {
		m_blackboardComponent->SetValueAsInt("IndexDestination", 0);
		if(GoToInitialTower())
			return;
	}

	if (explorerPath.Num() <= 1)
		return;
	
	const int32 indexDestination = m_blackboardComponent->GetValueAsInt("IndexDestination");

	if (indexDestination == 0)
		targetPoinIndex -= 1;
	else					
		targetPoinIndex += 1;

	if (targetPoinIndex >= explorerPath.Num())
	{
		m_blackboardComponent->SetValueAsInt("IndexDestination", 0);
		targetPoinIndex = explorerPath.Num() - 1;
	}
	else if (targetPoinIndex < 0)
	{
		m_blackboardComponent->SetValueAsInt("IndexDestination", explorerPath.Num());
		targetPoinIndex = 0;
	}

	m_blackboardComponent->SetValueAsInt("TargetPointIndex", targetPoinIndex);

	const auto* nextPoint = explorerPath[targetPoinIndex];
	if (!nextPoint) return;

	m_blackboardComponent->SetValueAsVector("TargetPointPosition", nextPoint->GetActorLocation());
	
}

void AAICT_Explorer::CreateLight()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr || GetExplorerCharacter() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.CreateLight: Nullptr BlackboardComponent or ExplorerCharacter");
		return;
	}

	TArray<ATGPT_Explorer*> explorerPath = m_explorerCharacter->m_path;

	if (explorerPath.Num() <= 1)
		return;

	const FVector offset = (m_explorerCharacter->GetActorForwardVector() * 100.0f ) + (m_explorerCharacter->GetActorRightVector() * 100);
	m_blackboardComponent->SetValueAsBool("IsRunningFromPlayer", false);

	const int32 targetPoinIndex = m_blackboardComponent->GetValueAsInt("TargetPointIndex");

	// Check if the target is outside oh the array, is inside the light, or already has an energy torche
	if (targetPoinIndex >= explorerPath.Num()
		|| explorerPath[targetPoinIndex]->m_isInsideLight
		|| !m_explorerCharacter->m_path[targetPoinIndex]
		|| m_explorerCharacter->m_path[targetPoinIndex]->m_energyTorche != nullptr)
		return;

	// We have no energyTorche class
	if (m_explorerCharacter->m_energyTorche == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.CreateLight: No m_energyTorche on m_explorerCharacter");
		return;
	}
	
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	const FVector pos = m_explorerCharacter->GetActorLocation() - FVector(0.f, 0.f, 100.f) + offset;

	if (m_explorerCharacter->m_energyTorche.GetDefaultObject() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.CreateLight: No m_energyTorche DefaultObject");
		return;
	}

	AACTR_EnergyTorche* energyTorche = GetWorld()->SpawnActor<AACTR_EnergyTorche>(m_explorerCharacter->m_energyTorche.GetDefaultObject()->GetClass(), pos, FRotator(0.0f, 0.0f, 0.0f), ActorSpawnParams);
	if (energyTorche)
	{
		energyTorche->m_explorerOwner = m_explorerCharacter;
		explorerPath[targetPoinIndex]->m_energyTorche = energyTorche;
		CheckPathCompleted(explorerPath);
	}
}

void AAICT_Explorer::CheckPathCompleted(TArray<ATGPT_Explorer*> path) const
{
	if (path.Num() <= 0)
		return;

	bool isComplete = true;

	for(int i = 0; i < path.Num(); ++i)
	{
		if(i == 0 || i == path.Num() - 1)
		{
			auto* tower = path[i]->m_energyTower;
			if(tower != nullptr)
			{
				if (!tower->isLightOnPermanent()) {
					isComplete = false;
					break;
				}
			}
			else
			{
				isComplete = false;
				break;
			}
		}
		else
		{
			auto* torch = path[i]->m_energyTorche;
			if (!torch)
			{
				isComplete = false;
				break;
			}
		}
	}
	if(isComplete)
	{
		auto* tower = path[0]->m_energyTower;
		if (tower != nullptr)
			tower->AddConnectionCount(1);

		if (path.Num() <= 1)
			return;

		tower = path.Last()->m_energyTower;
		if (tower != nullptr)
			tower->AddConnectionCount(1);
	}
}

void AAICT_Explorer::RunAwayFromPlayer(bool isInFront)
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr || GetExplorerCharacter() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.RunAwayFromPlayer: Nullptr BlackboardComponent or ExplorerCharacter");
		return;
	}

	if (m_explorerCharacter->m_path.Num() <= 1 || m_explorerCharacter->GetCharacterMovement() == nullptr) {
		return;
	}

	m_explorerCharacter->GetCharacterMovement()->MaxWalkSpeed = m_explorerCharacter->m_runSpeed;

	int newDestination = 0;
	const TArray<ATGPT_Explorer*>& explorerPath = m_explorerCharacter->m_path;
	const int destination = m_blackboardComponent->GetValueAsInt("IndexDestination");
	int32 targetPoinIndex = m_blackboardComponent->GetValueAsInt("TargetPointIndex");
	
	m_blackboardComponent->SetValueAsBool("IsRunningFromPlayer", true);

	if (destination > 0) {
		newDestination = 0;
		if (isInFront) {
			targetPoinIndex--;
			m_blackboardComponent->SetValueAsInt("IndexDestination", newDestination);
		}
		else
		{
			targetPoinIndex++;
		}
	}
	else {
		newDestination = explorerPath.Num() - 1;
		if (isInFront) {
			targetPoinIndex++;
			m_blackboardComponent->SetValueAsInt("IndexDestination", newDestination);
		}
		else
		{
			targetPoinIndex--;
		}
	}

	if (targetPoinIndex >= explorerPath.Num()) {
		targetPoinIndex = explorerPath.Num() - 1;
	}
	else if (targetPoinIndex < 0) {
		targetPoinIndex = 0;
	}
	
	m_blackboardComponent->SetValueAsInt("TargetPointIndex", targetPoinIndex);
	m_blackboardComponent->SetValueAsVector("TargetPointPosition", explorerPath[targetPoinIndex]->GetActorLocation());
}

void AAICT_Explorer::ShootFlare()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr || GetExplorerCharacter() == nullptr || m_explorerCharacter->m_flareClass == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.ShootFlare: Nullptr BlackboardComponent or ExplorerCharacter, or m_flareClass has no class");
		return;
	}

	FVector newPos = FVector::ZeroVector;
	for (int i = 0; i < indexBuffer; i++)
	{
		newPos += m_velocityHistoryBuffer[i];
	}

	const FVector average = newPos / indexBuffer;
	BallisticProjectileCalculations(m_explorerCharacter, average);

	//m_blackboardComponent->SetValueAsBool("PlayerNear", false);
	m_blackboardComponent->SetValueAsBool("IsRunningFromPlayer", false);
}

void AAICT_Explorer::BallisticProjectileCalculations(ACHRT_Explorer* explorer, FVector average)
{
	if (explorer == nullptr)
		return;

	//FIXME este codigo necesita explicación, es completamente opaco
	const FVector playerPos = BLKB_Global::getPlayerLocation();
	const float flareSpeed = explorer->m_flareClass.GetDefaultObject()->m_speed;
	const FVector offset = explorer->GetActorForwardVector() * 40;
	const FVector spawnLoc = explorer->GetActorLocation() + offset;


	const float a = FVector::DotProduct(average, average) - (flareSpeed * flareSpeed);
	const float b = 2 * FVector::DotProduct(playerPos - spawnLoc, average);
	const FVector aux = playerPos - spawnLoc;
	const float c = FVector::DotProduct(aux, aux);
	float result1 = 0, result2 = 0;

	QuadraticEquation(a, b, c, result1, result2);
	result1 = fmax(result1, result2);
	if (result1 == -1) {
		//USYST_LogManager::PrintError(this, "AAICT_Explorer.BallisticProjectileCalculations: Not solve");
		return;
	}

	const FVector targetPoint = playerPos + average * result1;

	const float horizontalDist = FVector::DistXY(targetPoint, spawnLoc);
	ParabolicCalculations(explorer, playerPos, targetPoint, spawnLoc, flareSpeed);
	//m_velocityHistoryBuffer.Empty();
}

void AAICT_Explorer::QuadraticEquation(float a, float b, float c, float& result1, float& result2)
{
	result1 = -1;
	result2 = -1;

	//FIXME este codigo necesita explicación, es completamente opaco
	const double insideSqrt = (b * b) - (4 * a * c);
	if (insideSqrt < 0 || a == 0)
	{
		//USYST_LogManager::PrintError(this, "AAICT_Explorer.QuadraticEquation: insideSqrt < 0 or a == 0");
		return;
	}

	const double sqrtResult = sqrt(insideSqrt);
	const double denominator = 2 * a;

	result1 = ((-b) + sqrtResult) / denominator;
	result2 = ((-b) - sqrtResult) / denominator;
}

void AAICT_Explorer::ParabolicCalculations(ACHRT_Explorer* explorer, FVector playerPosNoZ, FVector targetPoint, FVector spawnLoc, const float& flareSpeed)
{
	if (explorer == nullptr || explorer->m_flareClass.GetDefaultObject() == nullptr || flareSpeed == 0)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.QuadraticEquation: explorer == nullptr, explorer->m_flareClass.GetDefaultObject() == nullptr or flareSpeed == 0");
		return;
	}

	//FIXME este codigo necesita explicación, es completamente opaco
	const float flareSpeedSquared = flareSpeed * flareSpeed;
	const float horizontalDist = FVector::DistXY(targetPoint, spawnLoc);

	const float verticalDist = targetPoint.Z - explorer->GetActorLocation().Z;
	const float gravity = explorer->m_flareClass.GetDefaultObject()->m_gravity;

	const float a = -(gravity * (horizontalDist * horizontalDist)) / (2 * flareSpeedSquared);
	const float b = horizontalDist;
	const float c = -((gravity * (horizontalDist * horizontalDist)) / (2 * flareSpeedSquared)) - verticalDist;
	float result1 = 0, result2 = 0;
	
	QuadraticEquation(a, b, c, result1, result2);
	result1 = fmax(result1, result2);
	if (result1 == -1) {
		//USYST_LogManager::PrintError(this, "AAICT_Explorer.QuadraticEquation: explorer == nullptr, explorer->m_flareClass.GetDefaultObject() == nullptr or flareSpeed == 0");
		return;
	}

	const float angle = atan(result1); // in radians
	const float horizontalSpeed = flareSpeed * cos(angle);
	const float verticalSpeed = flareSpeed * sin(angle);

	const FVector AI2Player = targetPoint - spawnLoc;
	FVector horizontalDistVector = FVector(AI2Player.X, AI2Player.Y, 0.0f);

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	horizontalDistVector.Normalize();
	const FVector velocity = horizontalDistVector * horizontalSpeed + FVector(0.0f, 0.0f, verticalSpeed);

	const FRotator spawnRot = UKismetMathLibrary::FindLookAtRotation(explorer->GetActorLocation(), targetPoint);
	explorer->FaceRotation(spawnRot);

	auto* spawned = GetWorld()->SpawnActor<AActor>(explorer->m_flareClass, spawnLoc, spawnRot, ActorSpawnParams);
	if (spawned == nullptr)
		return;

	AACTR_Flare* flare = Cast<AACTR_Flare>(spawned);
	if (flare != nullptr)
	{
		flare->SetActorLocation(spawnLoc);
		flare->m_velocity = velocity;
	}
}

void AAICT_Explorer::RotateTowardsPlayer()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetExplorerCharacter() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.ShootFlare: Nullptr ExplorerCharacter");
		return;
	}

	const auto& rotationToPlayer = UKismetMathLibrary::FindLookAtRotation(m_explorerCharacter->GetActorLocation(), BLKB_Global::getPlayerLocation());
	m_explorerCharacter->FaceRotation(rotationToPlayer);

	CheckSeePlayer();
}

void AAICT_Explorer::ChangeRunningState()
{
	// Check if the explorer has BlackboardComponent or ExplorerCharacter
	if (GetBlackboardComponent() == nullptr)
	{
		USYST_LogManager::PrintError(this, "AAICT_Explorer.ShootFlare: Nullptr BlackboardComponent");
		return;
	}

	m_blackboardComponent->SetValueAsBool("IsRunningFromPlayer", false);
}