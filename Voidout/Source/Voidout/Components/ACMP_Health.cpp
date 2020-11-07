
#include "ACMP_Health.h"
#include "Character/CHRT_Player.h"
#include "Character/CHRT_Commander.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SoundEventManager.h"
#include "System/SYST_SystemsManager.h"

#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollection.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UACMP_Health::UACMP_Health()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UACMP_Health::Kill()
{
	m_currentHp = 0;
	return UpdateDead();
}

// Take damage. 0 -> Nothing, 1 -> Damaged, 2 -> Dead
int UACMP_Health::TakeDamage(float damage)
{
	if (!m_invincible && damage > 0.f)
	{
		// If you get hurt the time without damage is restarted.
		m_timeWithoutDamage = 0;
		m_currentHp = fmax(m_currentHp - damage, 0);
		return UpdateDead() ? M_DEAD : M_DAMAGED;
	}
	return M_NOTHING;
}

bool UACMP_Health::UpdateDead()
{
	return m_dying = m_currentHp <= 0.f;
}

//FIXME improve
// Called every frame
void UACMP_Health::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ACHRT_Player* player = Cast<ACHRT_Player>(GetOwner());
	if (player != nullptr)
	{
		//Show the screen damage feedback.
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), m_materialDamageCollection, FName(TEXT("Health")), fmax((m_currentHp) / m_maxHp, 0));

		m_timeWithoutDamage += DeltaTime;
		if (m_timeWithoutDamage >= m_timeUntilPlayerRecovers)
			//If is the player, he can recover his life
			LifeRecoverByTime(DeltaTime);
	}
	else
	{
		//If an enemy has died and is in ragdoll mode for than m_timeCorpseToDisapear seconds, it disapears.
		if (m_dying)
		{
			m_timeEnemyDead += DeltaTime;
			if (m_timeEnemyDead >= m_timeCorpseToDisappear)
				GetOwner()->Destroy();
		}
		else
		{
			ACHRT_Commander* commander = Cast<ACHRT_Commander>(GetOwner());
			if (commander != nullptr && m_energyShield != nullptr)
				//FIXME Magic numbers, otherwise it doesn't properly work
				m_energyShield->SetVisibility(m_currentHp > 100.f);
		}
	}
}

void UACMP_Health::LifeRecoverByTime(float deltaTime)
{
	LifeRecover(m_healingRate * m_timeWithoutDamage * deltaTime);
}

void UACMP_Health::LifeRecover(float hp)
{
	if (m_canRecover && m_currentHp < m_maxHp)
		m_currentHp = fmin(m_currentHp + hp, m_maxHp);
}
