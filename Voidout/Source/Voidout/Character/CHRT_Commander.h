
#pragma once

#include "CoreMinimal.h"
#include "CHRT_Enemy.h"
#include "CHRT_Commander.generated.h"

UCLASS()
class VOIDOUT_API ACHRT_Commander : public ACHRT_Enemy
{
	GENERATED_BODY()

	// Is player on range?
	bool IsPlayerOnRange() const noexcept;

	// Child class
	TSubclassOf<class ACHRT_Enemy> m_childrenClass{ nullptr };
public:

	// Sets default values for this character's properties
	ACHRT_Commander();

	// Create the children
	void CreateChildren();
	
	// Begin play.
	void BeginPlay() override;

	// UpdateChildren
	void UpdateChildren() noexcept;

	// Patrol path.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol") TArray<AActor*> m_path{};

	// Segs to recover.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExtraLife")
	float m_segsToHeal{ 15.0f };

	// Current segs.
	float m_sumSegs{ 0.f };

	// Max hp.
	float m_maxHp{ 200.f };
	
	// Damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float m_damage{ 30.f };

	// Distance to hit.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float m_distToHit{ 200.f };
	
	// Normal move.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float m_walkingSpeed{ 600.f };
	
	// Normal move.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float m_runningSpeed{ 900.f };

	// Number of children to spawn.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AICommander")
	int m_nChildren{ 3 };

	// Spawn position
	FVector m_spawnPos{};

	// Radius to spawn children
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AICommander")
	float m_spawnChildrenRadius{ 400.0f };

	// Array of children.
	UPROPERTY() TArray<ACHRT_Enemy*> m_children{};
	
	// Attack
	UFUNCTION(BlueprintCallable)
	void Attack() const noexcept;

	// Do damages.
	UFUNCTION(BlueprintCallable)
	bool DoDamage() const noexcept;

	virtual int TakeDamage(float damage) override;

	virtual void Death() override;
};
