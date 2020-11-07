
#include "CHRT_Commander.h"
#include "AIController/BLKB_Global.h"
#include "Animations/ANIM_Commander.h"
#include "Assets/ACTR_EnergyBarrier.h"
#include "Character/CHRT_Player.h"
#include "Components/ACMP_Health.h"
#include "System/SYST_SystemsManager.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"


ACHRT_Commander::ACHRT_Commander()
{
	// Retrieve the children class
	static ConstructorHelpers::FClassFinder<ACHRT_Enemy> minionBP_Class(TEXT("/Game/Blueprints/Character/CHRT_Minion_BP"));
	m_childrenClass = minionBP_Class.Class;
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (m_health != nullptr)
		m_health->m_currentHp = m_health->m_maxHp = m_maxHp;

	if (GetCharacterMovement() == nullptr)
		return;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = { 0.0f, 640.0f, 0.0f };
}

// Begin play.
void ACHRT_Commander::BeginPlay()
{
	Super::BeginPlay();

	m_spawnPos = GetActorLocation();
	
	CreateChildren();
}

// UpdateChildren
void ACHRT_Commander::UpdateChildren() noexcept
{
	const int childsToDelete{ m_children.Num() - m_nChildren };

	for(auto i = 0; i < childsToDelete; ++i)
	{
		m_children[i]->Destroy();
	}
}

void ACHRT_Commander::CreateChildren()
{
	// Reserve space (memory) in the array for the children
	m_children.Empty();
	m_children.Reserve(m_nChildren);

	// Get the location and rotation of the commander as reference to spwan
	const FVector loc = GetActorLocation();
	const FRotator rot = GetControlRotation();

	// Create every child
	for (auto i = 0; i < m_nChildren; ++i)
	{
		// The child is spawned in a random point of a circle with the configured radius, with the commander as center
		ACHRT_Enemy* child = Cast<ACHRT_Enemy>(UAIBlueprintHelperLibrary::SpawnAIFromClass(GetWorld(), m_childrenClass, nullptr, GetRandomPointInCircle(loc, m_spawnChildrenRadius), rot));

		if (!child) continue;

		// Add the child to the collection, and set the parent as target
		m_children.Add(child);
		child->setTarget(this);
	}
}

// Is player on range?
bool ACHRT_Commander::IsPlayerOnRange() const noexcept
{
	const auto plyLoc = BLKB_Global::getPlayerLocation();
	const auto distToPlayer = FVector::Dist(GetActorLocation(), plyLoc);
	return distToPlayer < m_distToHit;
}

void ACHRT_Commander::Attack() const noexcept
{
	const bool canHitPlayer{ IsPlayerOnRange() };

	if (GetMesh() != nullptr)
	{
		auto* commAnimation = Cast<UANIM_Commander>(GetMesh()->AnimScriptInstance);
		if (commAnimation != nullptr)
			commAnimation->SetIsAttacking(canHitPlayer);
	}
}

bool ACHRT_Commander::DoDamage() const noexcept
{
	const bool canHitPlayer{ IsPlayerOnRange() };

	if (!canHitPlayer)
		return false;
	
	auto* player = Cast<ACHRT_Player>(BLKB_Global::getPlayerActor());
	if (player != nullptr)
		player->TakeDamage(m_damage);

	return true;
}

int ACHRT_Commander::TakeDamage(float damage)
{
	if (IsDying())
		return 0;

	return Super::TakeDamage(damage);
}

void ACHRT_Commander::Death()
{

	if (m_energyBarrierAsigned != nullptr) {
		m_energyBarrierAsigned->UnsubscribeToWall();
	}

	if (GetMesh() != nullptr)
	{
		//Transform the enemy mesh in ragdoll
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetAllBodiesSimulatePhysics(true);
	}

	for (auto* child : m_children)
	{
		if (child != nullptr && child->m_health != nullptr)
			child->TakeDamage(100.f);
	}

	Super::Death();
}