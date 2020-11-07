
#include "ACMP_Spawn_Explorer.h"
#include "Assets/ACTR_EnergyTower.h"
#include "Assets/ACTR_SwitchableLight.h"
#include "Components/ACMP_LightStructureSubscription.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Character/CHRT_Explorer.h"
#include "TimerManager.h"
#include "System/SYST_LogManager.h"

// Sets default values for this component's properties
UACMP_Spawn_Explorer::UACMP_Spawn_Explorer()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UACMP_Spawn_Explorer::SpawnExplorer(ATGPT_Explorer* initialPoint)
{
	auto* owner = GetOwner();
	if (owner == nullptr)
		return;

	AACTR_EnergyTower* energyTower = Cast<AACTR_EnergyTower>(owner);
	if (energyTower == nullptr || energyTower->isLightOnPermanent())
		return;

	m_point = initialPoint;

	float r1 = energyTower->getlightArea(); //Radio de luz
	if(r1 == 0)
		r1 = 500;
	
	if (!energyTower->isLightOnVoidout())
		m_time_to_spawn = energyTower->m_timeWithLightOff;
	
	float x = 0.f;
	float y = 0.f;

	while (FVector::Dist(energyTower->GetActorLocation(), m_newPos) > r1) {
		x = RandomFloat(energyTower->GetActorLocation().X - r1, energyTower->GetActorLocation().X + r1);
		y = RandomFloat(energyTower->GetActorLocation().Y - r1, energyTower->GetActorLocation().Y + r1);
		m_newPos = FVector(x, y, 100);
	}
	
	UACMP_LightStructureSubscription* towerSubscription = GetOwner()->FindComponentByClass<UACMP_LightStructureSubscription>();
	
	if(m_explorer && towerSubscription && towerSubscription->m_isActive)
		GetWorld()->GetTimerManager().SetTimer(m_timerHandler, this, &UACMP_Spawn_Explorer::spawnAfterTime, m_time_to_spawn, false);
}

void UACMP_Spawn_Explorer::spawnAfterTime()
{
	APawn* spawnedExplorer = UAIBlueprintHelperLibrary::SpawnAIFromClass(GetWorld(), m_explorer.GetDefaultObject()->GetClass(), nullptr, m_newPos);
	if (spawnedExplorer != nullptr)
	{
		ACHRT_Explorer* explorer = Cast<ACHRT_Explorer>(spawnedExplorer);
		if (explorer != nullptr)
		{
			TArray<ATGPT_Explorer*> path;
			path.Add(m_point);
			explorer->m_path = path;
		}
	}
	else
		USYST_LogManager::PrintString(this, "Error spawning explorer", true, true, FColor::Red);
}

// Used to generate random number in a range
float UACMP_Spawn_Explorer::RandomFloat(float a, float b)
{
	const float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	const float diff = b - a;
	const float r = random * diff;

	return a + r;
}