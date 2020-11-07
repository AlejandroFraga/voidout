
#include "BLKB_Global.h"
#include "Character/CHRT_Player.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

// Ctor.
BLKB_Global::BLKB_Global()
{
	const auto& wContxts = GEngine->GetWorldContexts();
	ACHRT_Player* ch{ nullptr };
	UWorld* world{ nullptr };

	for (const auto& ctxt : wContxts) // Lookup on all world contexts.
	{
		APawn* pawn = UGameplayStatics::GetPlayerPawn(ctxt.World(), 0);
		if (pawn == nullptr)
			continue;

		ch = Cast<ACHRT_Player>(pawn); // Cast

		if (ch != nullptr) // If not null.
		{
			world = ctxt.World();
			m_mainCh = ch;
			break;
		}
	}
}

// Update player last position by explorer bullet.
void BLKB_Global::updateLastPlayerPositionByExplorerBullet(const float deltaTime) noexcept
{
	if (!getInst().m_playerSeenByFlare)
		return;

	getInst().m_timerLastPlayerPos -= deltaTime;
	updateLastPlayerPosition();

	if (getInst().m_timerLastPlayerPos <= 0.0f)
	{
		getInst().m_timerLastPlayerPos = getInst().m_defaultTimerLastPlayerPos;
		getInst().m_playerSeenByFlare = false;
	}
}


// Find if we are on the Engine.
AActor* BLKB_Global::findIfEngine()
{
	if (GEngine == nullptr)
		return nullptr;

	const auto& wContxts = GEngine->GetWorldContexts();
	APawn* ch{ nullptr };

	for (const auto& ctxt : wContxts) // Lookup on all world contexts.
	{
		ch = UGameplayStatics::GetPlayerPawn(ctxt.World(), 0); // Cast

		if (ch != nullptr) // If not null.
			return ch;
	}
	
	return nullptr;
}

ACHRT_Player* BLKB_Global::getPlayer() noexcept
{
	auto* acEngine = findIfEngine();
	if (acEngine != nullptr)
		return Cast<ACHRT_Player>(acEngine);

	return getInst().m_mainCh;
}


AActor* BLKB_Global::getPlayerActor() noexcept
{
	auto* acEngine = findIfEngine();
	if (acEngine != nullptr)
		return acEngine;

	return getInst().m_mainCh;
}

// Update player last position.
void BLKB_Global::updateLastPlayerPosition() noexcept
{
	if(getInst().m_mainCh != nullptr)
		getInst().m_playerLastPos = getPlayerLocation();
}

// Get player location.
FVector BLKB_Global::getPlayerLocation() noexcept
{
	if (getInst().getPlayerActor() != nullptr)
		return getInst().getPlayerActor()->GetActorLocation();

	return FVector::ZeroVector;
}

//Set time flare make player visible
void BLKB_Global::setDefaultTimerLastPlayerPos(const float defaultTimerLastPlayerPos) noexcept
{
	getInst().m_defaultTimerLastPlayerPos = defaultTimerLastPlayerPos;
	getInst().m_timerLastPlayerPos = defaultTimerLastPlayerPos;
}