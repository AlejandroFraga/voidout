
#pragma once

#include "CoreMinimal.h"
#include "CHRT_AliveCharacter.h"
#include "CHRT_Enemy.generated.h"

UCLASS()
class VOIDOUT_API ACHRT_Enemy : public ACHRT_AliveCharacter
{
	GENERATED_BODY()

public:

	// Parent (to follow).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	const AActor* m_parent{ nullptr };

	// Energy barrier asigned.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnergyBarrier")
	class AACTR_EnergyBarrier* m_energyBarrierAsigned{ nullptr };

	// Unshoot threshold
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIMinion")
	float m_dontShootThr{ 0.1f };
	
	// Success shoot threshhold
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIMinion")
	float m_successShootThrOriginal{ 0.75f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIMinion")
	float m_successShootThr{ m_successShootThrOriginal };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIMinion")
	float m_successFlaredThr{ 0.85f };

	// Sets default values for this character's properties
	ACHRT_Enemy();

	//If an enemy is subscribed to an Energy tower, when he dies, he must unsuscribe from that energy tower.
	void EnemyTowerUnsubscribe();
	
	// Set taget to follow.
	void setTarget(const AActor* target) { m_parent = target; }

	// Subscribe me to a tower.
	void SubscribeTo(const AActor* actTo);

	// Get random point in circle.
	static FVector GetRandomPointInCircle(const FVector& origin, float radius) noexcept;

	bool IsDying();

	virtual int TakeDamage(float damage);

	virtual void Kill();

	virtual void Death();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Death | Sound")
	void PlayDeathSound();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Death | NPS")
	void PlayDeathNPS();

	void PlayDeathPS();
};
