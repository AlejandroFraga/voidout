
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <tuple>
#include "AICT_Minion.generated.h"

UCLASS()
class VOIDOUT_API AAICT_Minion : public AAIController
{
	GENERATED_BODY()

	// ---------------------------------------------------- Variables.

	UBlackboardComponent* m_blackboardComponent{ nullptr };

	// ---------------------------------------------------- Methods.

	void CheckPlayerHP() noexcept;

	// Get the blackboard component
	UBlackboardComponent* GetBlackboardComponent();

public:

	// ---------------------------------------------------- Variables.

	// Spawning object
	TSubclassOf<class AACTR_SoldierBullet> m_shootingProjectile{ nullptr };

	//Distance to atack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AISoldier")
	float m_maxPlayerDistance{ 90000.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AISoldier")
	float m_timeBetweenShoots{ 0.5f };

	// ---------------------------------------------------- Methods.
	
	// Ctor.
	AAICT_Minion();

	// Begin Play.
	void BeginPlay() override;

	// Check nearby player.
	virtual bool CheckNearbyPlayer() const;

	// Get player distance
	float GetCharacterDistance() const;
	
	// Move
	void Move();
	
	// Shoot.
	void Shoot();

	// Focus player.
	void Focus();

	// Reset focus.
	void ResetFocus();

	// Solve Quadratic
	static std::tuple<bool, float, float> SolveQuadratic(float a, float b, float c) noexcept;
	
};
