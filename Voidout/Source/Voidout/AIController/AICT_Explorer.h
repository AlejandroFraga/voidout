
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CircularBuffer.h"
#include "AICT_Explorer.generated.h"

UCLASS()
class VOIDOUT_API AAICT_Explorer : public AAIController
{
	GENERATED_BODY()

	// ---------------------------------------------------- Variables.

	UBlackboardComponent* m_blackboardComponent{ nullptr };

	class ACHRT_Explorer* m_explorerCharacter{ nullptr };

	//TArray<FVector> m_velocityHistoryBuffer = TArray<FVector>();
	TCircularBuffer<FVector> m_velocityHistoryBuffer = TCircularBuffer<FVector>(50 );

	int32 indexBuffer = 0;

	// ---------------------------------------------------- Methods.

	// Get the blackboard component
	UBlackboardComponent* GetBlackboardComponent();

	// Get commander character
	ACHRT_Explorer* GetExplorerCharacter();

	void CheckTowers();

	void CheckSeePlayer();

	void CheckOtherEnemyNearby();

	float RandomFloat(float a, float b) const;

	bool GoToInitialTower();

	void BallisticProjectileCalculations(ACHRT_Explorer* explorer, FVector average);

	void QuadraticEquation(float a, float b, float c, float& x1, float& x2);

	void ParabolicCalculations(ACHRT_Explorer* explorer, FVector playerPosNoZ, FVector targetPoint, FVector spawnLoc, const float& flareSpeed);

	void CheckPathCompleted(TArray<class ATGPT_Explorer*> path) const;

protected:

	// ---------------------------------------------------- Methods.

	// Called when the game starts or when spawned
	void BeginPlay() override;
	
public:

	// ---------------------------------------------------- Methods.
	
	//Check if the player is nearby
	UFUNCTION(BlueprintCallable, Category = "ExplorerAI")
	void CheckNearbyEnemy();

	//Get the next point in the path
	UFUNCTION(BlueprintCallable, Category = "ExplorerAI")
	void GetNextTargetPoint();
	
	//Spawn a lamp post
	UFUNCTION(BlueprintCallable, Category = "ExplorerAI")
	void CreateLight();

	//Get a point to run away from player
	UFUNCTION(BlueprintCallable, Category = "ExplorerAI")
	void RunAwayFromPlayer(bool isInFront);

	//Shoot the flare
	UFUNCTION(BlueprintCallable, Category = "ExplorerAI")
	void ShootFlare();
	
	//When shooting the player, rotate towards him
	UFUNCTION(BlueprintCallable, Category = "ExplorerAI")
	void RotateTowardsPlayer();

	//Change the bool running from player
	UFUNCTION(BlueprintCallable, Category = "ExplorerAI")
	void ChangeRunningState();
};
