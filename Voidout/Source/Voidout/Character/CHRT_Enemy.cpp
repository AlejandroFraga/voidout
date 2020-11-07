
#include "CHRT_Enemy.h"
#include "Components/ACMP_Health.h"
#include "Components/ACMP_LightStructureSubscription.h"
#include "System/SYST_LogManager.h"

#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "Misc/OutputDeviceNull.h"


// Sets default values
ACHRT_Enemy::ACHRT_Enemy()
{
	PrimaryActorTick.bCanEverTick = false;

	m_health = FindComponentByClass<UACMP_Health>();

	Tags.AddUnique("save");
}

// Subscribe me to a tower.
void ACHRT_Enemy::SubscribeTo(const AActor* actTo)
{
	// If already subscribed.
	if(actTo == nullptr || m_parent != nullptr)
		return;

	m_parent = actTo;

	UActorComponent* actorComp = m_parent->GetComponentByClass(UACMP_LightStructureSubscription::StaticClass());
	if(actorComp == nullptr)
		return;

	auto* triggerCmp = Cast<UACMP_LightStructureSubscription>(actorComp);
	if (triggerCmp == nullptr)
		return;

	triggerCmp->SubscribeEnemyToTower();
}

FVector ACHRT_Enemy::GetRandomPointInCircle(const FVector& origin, float radius) noexcept
{
	// We get a random point in the circle
	const auto angle = rand() % 360;
	const auto x = origin.X + radius * cosf(angle);
	const auto y = origin.Y + radius * sinf(angle);

	return { x, y, origin.Z };
}

bool ACHRT_Enemy::IsDying()
{
	return m_health == nullptr || m_health->m_dying;
}

int ACHRT_Enemy::TakeDamage(float damage)
{
	if (IsDying())
		return 0;

	return Super::TakeDamage(damage);
}

void ACHRT_Enemy::Kill()
{
	if (IsDying())
		return;

	Super::Kill();
}

void ACHRT_Enemy::Death()
{
	Super::Death();

	//he must unsuscribe from hes tower, if he is subscribe to anyone.
	EnemyTowerUnsubscribe();

	if (GetController() != nullptr)
	{
		AAIController* aiController = UAIBlueprintHelperLibrary::GetAIController(this);
		if (aiController != nullptr)
		{
			aiController->StopMovement();
			aiController->SetFocus(nullptr);
			if (aiController->GetBrainComponent() != nullptr)
			{
				aiController->GetBrainComponent()->Deactivate();
			}
		}
	}

	PlayDeathSound();

	PlayDeathNPS();

	//I call the hit animation (in the animation blueprint).
	UAnimInstance* animInstance = GetMesh() != nullptr ? GetMesh()->AnimScriptInstance : nullptr;
	if (animInstance != nullptr)
	{
		FOutputDeviceNull ar; //Needed to call a blueprint function
		animInstance->CallFunctionByNameWithArguments(TEXT("AnimDeath"), ar, nullptr, true);
	}
}

void ACHRT_Enemy::EnemyTowerUnsubscribe()
{
	//If for anything a tower has not been assigned to the enemy, I control it here, so nothing happens and it doesn't explode when the enemy is killed.
	const AActor* enemyTower = m_parent;
	if (enemyTower != nullptr)
	{
		UACMP_LightStructureSubscription* subscribedLight = enemyTower->FindComponentByClass<UACMP_LightStructureSubscription>();
		if (subscribedLight != nullptr)
			subscribedLight->UnsuscribeEnemyFromTower(this);//Unsubscribe the enemy from hes tower.
	}
}