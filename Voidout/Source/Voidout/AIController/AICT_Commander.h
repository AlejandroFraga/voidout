
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AICT_Commander.generated.h"

 UCLASS()
class VOIDOUT_API AAICT_Commander : public AAIController
{
	GENERATED_BODY()

	// Get a random point in a circle with m_spawnChildrenRadius radius and the commander as center
	static FVector GetRandomPointInCircle(const FVector& origin, float radius) noexcept;

 public:
	// ---------------------------------------------------- Methods.
	// Ctor.
	AAICT_Commander();

	// Tick override
	void Tick(float DeltaTime) override;

	// Move the minions with the commander
	void MoveMinions();

 	// Set the target path.
	void SetTargetPath();

	// Get the next target point.
	void GetNextPoint();

 	// Chase the player.
	void ChasePlayer();

	// Bassic attack.
	void Attack() const noexcept; //

	// ---------------------------------------------------- Variables.

	// Path to follow.
	UPROPERTY() TArray<AActor*> m_path;

	// Path size.
	int m_pathSize{ 0 };

	// Next idx.
	int m_nextPathPoint{ 0 };

	// Increment.
	int m_pathIncrement{ 1 };
};

 