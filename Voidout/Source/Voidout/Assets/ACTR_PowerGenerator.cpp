
#include "ACTR_PowerGenerator.h"
#include "Character/CHRT_Player.h"
#include "Components/ACMP_Health.h"
#include "Components/ACMP_LightStructureSubscription.h"
#include "System/SYST_SystemsManager.h"

#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


AACTR_PowerGenerator::AACTR_PowerGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	m_health = FindComponentByClass<UACMP_Health>();
}

bool AACTR_PowerGenerator::Death()
{
	UACMP_LightStructureSubscription* subscription = FindComponentByClass<UACMP_LightStructureSubscription>();
	if (subscription != nullptr)
	{
		//Check the number of towers subscribed if it's more than 0, the generator can't be destroyed.
		if (subscription->m_isActive)
		{
			CantDestroy();
			return false;
		}
		for (AACTR_EnergyBarrier* barrier : subscription->m_energyBarrier)
		{
			if (barrier != nullptr)
				barrier->UnsubscribeToWall();
		}
	}

	if (m_health != nullptr)
		m_health->DestroyComponent();

	SYST_SystemsManager::GetSoundEventManager()->playPowerGeneratorDestruction(GetActorLocation());

	return true;
}

void AACTR_PowerGenerator::CantDestroy()
{
	ACHRT_Player* playerCHRT = Cast<ACHRT_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (playerCHRT != nullptr)
	{
		playerCHRT->TakeDamage(repulsionDamage);

		FRotator initialRotation = playerCHRT->GetActorRotation();
		FRotator rotation = UKismetMathLibrary::FindLookAtRotation(playerCHRT->K2_GetActorLocation(), GetActorLocation());
		FVector forward = UKismetMathLibrary::GetForwardVector(rotation);

		const int jumpForce = 100;

		UPawnMovementComponent* playerMovement = playerCHRT->GetMovementComponent();
		if (playerMovement != nullptr)
			playerMovement->Velocity = FVector(-forward.X * repulsionForce, -forward.Y * repulsionForce, jumpForce);
	}
}