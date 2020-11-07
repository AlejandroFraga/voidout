
#include "CHRT_AliveCharacter.h"
#include "Components/ACMP_Health.h"
#include "System/SYST_LogManager.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
ACHRT_AliveCharacter::ACHRT_AliveCharacter()
{
	m_health = CreateDefaultSubobject<UACMP_Health>(TEXT("ACMP_Health"));
}

bool ACHRT_AliveCharacter::IsDying()
{
	return m_health == nullptr || m_health->m_dying;
}

void ACHRT_AliveCharacter::SetCanRecover(bool canRecover)
{
	if (m_health != nullptr)
		m_health->SetCanRecover(canRecover);
}

void ACHRT_AliveCharacter::LifeRecover(float hp)
{
	if (m_health != nullptr)
		m_health->LifeRecover(hp);
}

int ACHRT_AliveCharacter::TakeDamage(float damage)
{
	int result = 0;

	if (IsDying())
		return result;

	if (m_health != nullptr)
	{
		result = m_health->TakeDamage(damage);
		if (result == 2)
			Death();
	}

	return result;
}

void ACHRT_AliveCharacter::Kill()
{
	if (IsDying())
		return;

	if (m_health != nullptr && m_health->Kill())
		Death();
}

void ACHRT_AliveCharacter::Death()
{
	if (GetMesh() != nullptr)
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Destroys the capsule in order to not disturb while the enemy mesh is deleted.
	if (GetCapsuleComponent() != nullptr)
		GetCapsuleComponent()->DestroyComponent(true);
}