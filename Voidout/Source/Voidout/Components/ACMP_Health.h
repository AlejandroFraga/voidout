
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACMP_Health.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VOIDOUT_API UACMP_Health : public UActorComponent
{
	GENERATED_BODY()

	// ---------------------------------------------------- Variables.

	const int M_NOTHING = 0;

	const int M_DAMAGED = 1;

	const int M_DEAD = 2;

	//Time Without Damage
	float m_timeWithoutDamage{ 0.f };

	bool m_canRecover{ true };

	float m_timeEnemyDead{ 0.f };

	float m_timeCorpseToDisappear{ 2.f };

	// ---------------------------------------------------- Methods.

	void LifeRecoverByTime(float deltaTime);

public:
	// ---------------------------------------------------- Variables.

	// Total health.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float m_maxHp{ 100.f };

	// Current health.
	float m_currentHp{ m_maxHp };

	bool m_dying{ false };

	bool m_isReceivingDamage{ false };
	
	//time until the character begins to recover.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float m_timeUntilPlayerRecovers{ 2.f };

	//Healing rate.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float m_healingRate{ 15.f };

	//Necessary to be able to have the background material when the character takes damage (I assign it in the player's blueprint constructor)
	UPROPERTY(BlueprintReadWrite, Category = "HUD")
	class UMaterialParameterCollection* m_materialDamageCollection{ nullptr };

	//Necessary to be able to have the background material when the character takes damage (I assign it in the player's blueprint constructor)
	UPROPERTY(BlueprintReadWrite, Category = "Energy Shield")
	class UStaticMeshComponent* m_energyShield{ nullptr };

	UPROPERTY(BlueprintReadWrite, Category = "Health Component")
	bool m_invincible{ false };

	// ---------------------------------------------------- Methods.

	// Sets default values for this component's properties
	UACMP_Health();

	// Getters
	UFUNCTION(BlueprintCallable)
	inline float getMaxHp() const noexcept { return m_maxHp; }

	inline float getHp() const noexcept { return m_currentHp;}

	//Setters
	inline void SetCanRecover(bool canRecover){ m_canRecover = canRecover; }

	void LifeRecover(float hp);

	// Take damage.
	UFUNCTION(BlueprintCallable)
	int TakeDamage(float damage);

	UFUNCTION(BlueprintCallable)
	bool Kill();

	bool UpdateDead();

protected:

	// ---------------------------------------------------- Methods.

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};