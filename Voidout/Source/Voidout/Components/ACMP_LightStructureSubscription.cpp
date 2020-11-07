
#include "ACMP_LightStructureSubscription.h"
#include "System/SYST_LogManager.h"
#include "Assets/ACTR_SwitchableLight.h"
#include "Character/CHRT_Enemy.h"
#include "Character/CHRT_Commander.h"
#include "Character/CHRT_Explorer.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"


// Sets default values for this component's properties
UACMP_LightStructureSubscription::UACMP_LightStructureSubscription()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UACMP_LightStructureSubscription::BeginPlay()
{
	Super::BeginPlay();

	//I tell the lightwalls assigned to the tower that the tower is a child actor:
	for (AACTR_EnergyBarrier* barrier : m_energyBarrier)
	{
		if (barrier != nullptr)
			barrier->m_childActors.Add(GetOwner());
	}

	if (Cast<AACTR_PowerGenerator>(GetOwner())) { //If it is the generator I wait with a delay so it have time to load its components :
		const float timeToInitialize = 0.5f;
		FTimerHandle m_timerHandler;
		GetWorld()->GetTimerManager().SetTimer(m_timerHandler, this, &UACMP_LightStructureSubscription::subscribeTowerToGenerator, timeToInitialize, false);
		return;
	}

	m_collisionLightZone = Cast<USphereComponent>(GetOwner()->GetComponentsByTag(USphereComponent::StaticClass(), "LightZone")[0]);
	if (m_collisionLightZone != nullptr)
		m_collisionLightZone->OnComponentBeginOverlap.AddDynamic(this, &UACMP_LightStructureSubscription::OnLightZoneOverlapBegin);

	const float timeToInitialize = 1.0f;
	FTimerHandle m_timerHandler;
	GetWorld()->GetTimerManager().SetTimer(m_timerHandler, this, &UACMP_LightStructureSubscription::stopSubscribingEnemies, timeToInitialize, false);
}

void UACMP_LightStructureSubscription::stopSubscribingEnemies()
{
	m_stopSubscribingEnemies = true;
}

//I reset all tower values ​​when the player dies, so there are no problems detecting new enemies when they appear.
void UACMP_LightStructureSubscription::resetSubscriptionsOnLoadGame()
{
	m_generator.Empty();
	m_suscribedActors = 0;
	if (m_enemiesSubscribed.Num() <= 0)
		return;
	
	m_isActive = true;
	m_enemiesSubscribed.Empty();
}

//Only useful function to use with the towers (Detects all the enemies (soldiers or minions) that are in the tower to subscribe them):
void UACMP_LightStructureSubscription::OnLightZoneOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APawn* enemy = Cast<ACHRT_Enemy>(OtherActor);
	
	
	if(enemy == nullptr) 
		return;

	//If they are already subscribed to another light, or to that same one they cannot be subscribed (to avoid duplicates)
	if (m_enemiesSubscribed.Contains(OtherActor) || Cast<ACHRT_Enemy>(OtherActor)->m_parent != nullptr) 
		return;

	//IMPORTANT: Design department ONLY want to subscribe Minions and soldiers:
	if (!Cast<ACHRT_Commander>(enemy) && !Cast<ACHRT_Explorer>(enemy)) {

		m_enemiesSubscribed.Add(OtherActor);
		Cast<ACHRT_Enemy>(OtherActor)->m_parent = GetOwner();
		SubscribeEnemyToTower();
	}
}

void UACMP_LightStructureSubscription::SubscribeEnemyToTower()
{
	USYST_LogManager::PrintString(this, "DESDE TOWER: SUSCRITO",false);

	m_isActive = true;
	m_suscribedActors++; //increase subscribers

	//If the tower has any subscribers, I make sure to leave it on, and subscribe it to their assigned lightwalls.
	if (m_suscribedActors == 1)
	{
		if(Cast<AACTR_SwitchableLight>(GetOwner()) != nullptr)
			Cast<AACTR_SwitchableLight>(GetOwner())->LightOnPermanent();

		for (AACTR_EnergyBarrier* barrier : m_energyBarrier) {
			if (barrier != nullptr)
				barrier->SubscribeToWall();
		}
	}
}

//Unsubscribe: subtract one from the subscribers number and see if have to turn off the tower or not.
void UACMP_LightStructureSubscription::UnsuscribeEnemyFromTower(AActor* unsuscribedEnemy)
{
	USYST_LogManager::PrintString(this, "DESDE TOWER: UNSUSCRIBED", false);
	--m_suscribedActors;
	StopTower();
}

void UACMP_LightStructureSubscription::StopTower()
{
	if (m_suscribedActors > 0)
		return;
	
	m_isActive = false;

	//unsubscribe from all assigned lightwalls.
	if (Cast<AACTR_SwitchableLight>(GetOwner()) != nullptr)
		Cast<AACTR_SwitchableLight>(GetOwner())->LightOffPermanent(); //Off the light

	for (AACTR_EnergyBarrier* barrier : m_energyBarrier )
	{
		if (barrier != nullptr)
			barrier->UnsubscribeToWall();
	}
	
	if (m_generator.Num() <= 0)
		return;
	
	TArray< AACTR_PowerGenerator*> validGenerators;
	validGenerators.Reserve(m_generator.Num());

	//go through array and save the generators to those who the tower is subscribed and that are not null.
	for (AACTR_PowerGenerator* generator : m_generator)
	{
		if (generator != nullptr)
			validGenerators.Add(generator);
	}

	m_generator = validGenerators;
	validGenerators.Empty();

	//Unsuscribe the tower from all the asigned generators.
	for (AACTR_PowerGenerator* generator : m_generator)
	{
		if (generator != nullptr)
		{
			auto* generatorSubs = Cast<UACMP_LightStructureSubscription>(generator->GetComponentByClass(UACMP_LightStructureSubscription::StaticClass()));
			if (generatorSubs != nullptr)
				generatorSubs->unsuscribeTowerFromGenerator();
		}
	}
}


//Generators functions:
void UACMP_LightStructureSubscription::subscribeTowerToGenerator()
{
	USYST_LogManager::PrintString(this, "DESDE GENERATOR: SUSCRITO", false);

	//First check if there are not null towers:
	TArray< AACTR_EnergyTower*> validTowers;
	validTowers.Reserve(m_towers.Num());

	for (AACTR_EnergyTower* tower : m_towers) {
		if (tower == nullptr)
			return;

		validTowers.Add(tower);
	}

	m_towers = validTowers;
	validTowers.Empty();

	//Once I have the array with the valid towers, I already work with them:
	for (AACTR_EnergyTower* tower : m_towers) {
		if (tower == nullptr)
			return;

		if (!m_towerSubscribed.Contains(tower)) {
			m_towerSubscribed.Add(tower);

			m_isActive = true;
			++m_suscribedActors;

			if (tower->GetComponentByClass(UACMP_LightStructureSubscription::StaticClass()) != nullptr)
			{
				UACMP_LightStructureSubscription* towerSubs = Cast<UACMP_LightStructureSubscription>(tower->GetComponentByClass(UACMP_LightStructureSubscription::StaticClass()));
				if (towerSubs != nullptr && !towerSubs->m_generator.Contains(Cast<AACTR_PowerGenerator>(GetOwner()))) {
					towerSubs->m_generator.Add(Cast<AACTR_PowerGenerator>(GetOwner()));
				}
			}
		}
	}
}

void UACMP_LightStructureSubscription::unsuscribeTowerFromGenerator()
{
	USYST_LogManager::PrintString(this, "DESDE GENERATOR: UNSUSCRIBED", false);
	--m_suscribedActors;
	StopGenerator();
}

void UACMP_LightStructureSubscription::StopGenerator()
{
	if (m_suscribedActors == 0) {
		m_isActive = false;
	}
}
