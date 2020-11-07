
#pragma once

#include "CoreMinimal.h"
#include "AICT_Minion.h"
#include "AICT_Soldier.generated.h"

UCLASS()
class VOIDOUT_API AAICT_Soldier : public AAICT_Minion
{
	GENERATED_BODY()

	// ---------------------------------------------------- Variables.

	class ACHRT_Enemy* m_pawn{ nullptr };

	class UBlackboardComponent* m_blackboardComponent{ nullptr };

	class UAIPerceptionComponent* m_perceptionComponent{ nullptr };

	// Light array.
	TArray<class AActor*> m_lightArray{ nullptr };

	//Current Tower
	AActor* m_curTower{ nullptr };

	// IsShooting
	bool m_shooting{ false };

	// Is looking for a tower
	bool m_nextPoint{ false };

	//Enemy movement velocity
	float m_enemyVelocity {450.0f};

	//Distance to run away
	float m_maxRunDistance{ 70000.0f };

	//Time between shoots
	float m_timeBetweenShot{ 70000.0f };

	//Time between patrol movements
	float m_timeBetweenPatrols{ 2.0f };

	//Next point to run away
	float m_movementFactor{ 0.01f };

	//Current Enemy State
	int m_state{ -1 };

	//Attack velocity
	float m_attackVelocity {600.0f};

	//Patrol Velocity
	float m_patrolVelocity {300.0f};

	AActor* m_energyBarrier {nullptr};

	// ---------------------------------------------------- Methods.

	// Get the blackboard component
	void InitBlackboardComponent();

	// Get the perception component
	void InitPerceptionComponent();
	
protected:

	// ---------------------------------------------------- Methods.

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// ---------------------------------------------------- Methods.

	// Shoot action.
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Shoot();

	//Choose enemy velocity
	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void GoTo();

	//Check if need to attack
	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	bool CheckNearbyPlayer();

	//Patrol movement inside tower
	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void getNewPosition();

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	bool runAwayPlayer();

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void spawnBullet();

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void getNewTower();

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void initTowersArray();

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void suscribeToWall(APawn* wall);

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void SetTimeBetweenShoot(float time);

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void GetAttackAreaPoint();

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void SetAttackVelocity(float velocity) { m_attackVelocity = velocity; }

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void SetPatrolVelocity(float velocity) { m_patrolVelocity = velocity; }

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	void SetEnemyVelocity(float velocity) { m_enemyVelocity = velocity; }

	static float RandomFloat(float a, float b);

	static bool IsInRange(float min, float max, float value);

};
