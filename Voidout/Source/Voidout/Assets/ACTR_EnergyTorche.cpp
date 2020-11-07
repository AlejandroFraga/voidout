
#include "ACTR_EnergyTorche.h"
#include "Assets/TGPT_Explorer.h"
#include "Components/ACMP_Health.h"
#include "System/SYST_SystemsManager.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AACTR_EnergyTorche::AACTR_EnergyTorche()
{
	PrimaryActorTick.bCanEverTick = false;

	m_health = FindComponentByClass<UACMP_Health>();
}

// target point set energy Torche to null when destroyed
void AACTR_EnergyTorche::UpdateTargetPoint() const
{
	float dist = 0.0f;
	float shorterDist = 1000.0f;
	AActor* closestTarget = nullptr;
	TArray<AActor*> targets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATGPT_Explorer::StaticClass(), targets);
	for (auto i : targets)
	{
		dist = FVector::Dist(i->GetActorLocation(), GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());
		if (dist < shorterDist)
		{
			shorterDist = dist;
			closestTarget = i;
		}
	}
	ATGPT_Explorer* target = Cast<ATGPT_Explorer>(closestTarget);
	//if there isn't a energy Torche on the map that has not been set by an explorer...
	if (target != nullptr)
	{
		target->m_energyTorche = nullptr;
		target->m_haveLampPost = false;
	}
}

bool AACTR_EnergyTorche::Death()
{
	if (m_explorerOwner != nullptr && !m_explorerOwner->isDead)
		m_explorerOwner->DisablePathComplete();

	if (GetRootComponent() != nullptr)
	{
		auto attached = GetRootComponent()->GetAttachChildren();
		for (auto i : attached)
		{
			UParticleSystemComponent* particles = Cast<UParticleSystemComponent>(i);
			if (particles != nullptr)
				particles->DestroyComponent();
		}
	}

	UpdateTargetPoint();

	if (m_health != nullptr)
		m_health->DestroyComponent();

	SYST_SystemsManager::GetSoundEventManager()->playEnergyTorchDestruction(GetActorLocation());

	return true;
}