
#include "ACTR_DestroyableLight.h"
#include "Components/ACMP_Health.h"


// Sets default values
AACTR_DestroyableLight::AACTR_DestroyableLight()
{
	PrimaryActorTick.bCanEverTick = false;

	m_health = CreateDefaultSubobject<UACMP_Health>(TEXT("m_health"));
}

bool AACTR_DestroyableLight::Death()
{
	return m_health->Kill();
}