
#include "CHRT_Explorer.h"
#include "Assets/ACTR_EnergyTower.h"
#include "Assets/ACTR_Flare.h"
#include "Assets/ACTR_SwitchableLight.h"
#include "Assets/TGPT_Explorer.h"
#include "Components/ACMP_Spawn_Explorer.h"
#include "System/SYST_SystemsManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Assets/ACTR_EnergyTorche.h"
#include "Assets/ACTR_EnergyTower.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"


ACHRT_Explorer::ACHRT_Explorer()
{
	static ConstructorHelpers::FClassFinder<AACTR_Flare> ACTR_Flare_BPClass(TEXT("/Game/Blueprints/Assets/ACTR_Flare_BP"));
	m_flareClass = ACTR_Flare_BPClass.Class;

	static ConstructorHelpers::FClassFinder<AActor> ACTR_EnergyTorche_BPClass(TEXT("/Game/Blueprints/Assets/ACTR_EnergyTorche_BP"));
	m_energyTorche = ACTR_EnergyTorche_BPClass.Class;

	Tags.Add(TEXT("save"));
}

void ACHRT_Explorer::DisablePathComplete()
{
	if (m_path.Num() <= 1)
		return;

	bool isComplete = true;
	for (int i = 1; i < m_path.Num()-1; i++)
	{
		if (m_path[i] != nullptr)
		{
			AACTR_EnergyTorche* torch = m_path[i]->m_energyTorche;
			if (torch == nullptr)
			{
				isComplete = false;
				break;
			}
		}
	}
	if (isComplete)
	{
		if (m_path[0] != nullptr)
		{
			AACTR_EnergyTower* tower = m_path[0]->m_energyTower;
			if (tower == nullptr)
				return;

			AACTR_SwitchableLight* switchLight = Cast<AACTR_SwitchableLight>(tower);
			if (switchLight != nullptr)
				switchLight->AddConnectionCount(-1);
		}

		if (m_path[m_path.Num() - 1] != nullptr)
		{
			AACTR_EnergyTower* tower = m_path[m_path.Num() - 1]->m_energyTower;
			if (tower == nullptr)
				return;

			AACTR_SwitchableLight* switchLight = Cast<AACTR_SwitchableLight>(tower);
			if (switchLight != nullptr)
				switchLight->AddConnectionCount(-1);
		}
	}
}

int ACHRT_Explorer::TakeDamage(float damage)
{
	if (IsDying())
		return 0;

	return Super::TakeDamage(damage);
}

void ACHRT_Explorer::Death()
{
	if (GetMesh() != nullptr)
	{
		//Transform the enemy mesh in ragdoll
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetAllBodiesSimulatePhysics(true);
	}

	if (m_path.Num() <= 0)
		return;
	for (int i = 1; i < m_path.Num() - 1; i++)
	{
		if (m_path[i] != nullptr)
		{
			AACTR_EnergyTorche* torch = m_path[i]->m_energyTorche;
			if (torch != nullptr)
			{
				torch->m_explorerOwner = nullptr;
				if (torch->GetRootComponent() != nullptr)
				{
					auto attached = torch->GetRootComponent()->GetAttachChildren();
					for (auto attach : attached)
					{
						UParticleSystemComponent* particles = Cast<UParticleSystemComponent>(attach);
						if (particles != nullptr)
							particles->DestroyComponent();
					}
				}
			}
		}
	}
	isDead = true;

	// Get the last target point
	ATGPT_Explorer* lastTargetPoint = m_path.Last();
	// Check if it's valid
	if (lastTargetPoint == nullptr) return;
	// Get the last target point energy tower
	AACTR_EnergyTower* energyTower = lastTargetPoint->m_energyTower;
	if (energyTower != nullptr)
	{
		if (energyTower->isLightOnPermanent())
			energyTower->AddConnectionCount(-1);
	}
	// Get the last target point
	ATGPT_Explorer* firstTargetPoint = m_path[0];
	// Check if it's valid
	if (firstTargetPoint == nullptr) return;
	// Get the last target point energy tower
	energyTower = firstTargetPoint->m_energyTower;
	if (energyTower != nullptr)
	{
		if (energyTower->isLightOnPermanent())
			energyTower->AddConnectionCount(-1);
	}
	Respawn();

	Super::Death();
}

void ACHRT_Explorer::Respawn()
{
	if (m_path.Num() <= 0)
		return;

	ATGPT_Explorer* targetPoint = m_path[0];
	if (targetPoint == nullptr)
		return;

	const auto* energyTower = targetPoint->m_energyTower;
	if (energyTower == nullptr || !energyTower->isLightOnPermanent())
		return;

	UACMP_Spawn_Explorer* spawnComponent = energyTower->FindComponentByClass<UACMP_Spawn_Explorer>();
	if (spawnComponent != nullptr) {
		spawnComponent->SpawnExplorer(targetPoint);
		isDead = false;
	}
}