// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include <memory>

/**
 * 
 */
class VOIDOUT_API BLKB_Global
{
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
	
	// Light array.
	class TArray<class AActor*> m_lightArray;

	// Player pointer.
	class ACHRT_Player* m_mainCh{ nullptr };

	// Player last position.
	FVector m_playerLastPos{ FVector::ZeroVector };

	// Seen by explorer bullet.
	bool m_playerSeenByFlare{ false };

	// Default timer.
	float m_defaultTimerLastPlayerPos{ 30.0f };

	// Timer.
	float m_timerLastPlayerPos{ m_defaultTimerLastPlayerPos };

	// ---------------------------------------------------------------------- METHODS.
	// Instance.
	static BLKB_Global& getInst() noexcept
	{
		static const std::unique_ptr<BLKB_Global> instance{ new BLKB_Global() };
		return *instance;
	}

	// Ctor.
	BLKB_Global();

	// Update player last position by explorer bullet.
	static void updateLastPlayerPositionByExplorerBullet(const float deltaTime) noexcept;

	// Find if we are on the Engine.
	static AActor* findIfEngine();

public:	
	inline static TArray<AActor*> getLightArray() noexcept { return getInst().m_lightArray; } // Get light array.
	inline static FVector getLastSeenPlayerPosition() noexcept { return getInst().m_playerLastPos; } // Get last shawn position.

	// Get player.
	static ACHRT_Player* getPlayer() noexcept;
	static AActor* getPlayerActor() noexcept;

	// Get player location.
	static FVector getPlayerLocation() noexcept;

	// Update player last position.
	static void updateLastPlayerPosition() noexcept;

	// Set seen by player
	static void seenByFlare(bool seen) { getInst().m_playerSeenByFlare = seen; }

	// Get if seen by player
	static bool getSeenByFlare() { return getInst().m_playerSeenByFlare; }
	
	//Set time flare make player visible
	static void setDefaultTimerLastPlayerPos(const float defaultTimerLastPlayerPos) noexcept;

	//
	static TArray<AActor*> m_aliveActors;

};
