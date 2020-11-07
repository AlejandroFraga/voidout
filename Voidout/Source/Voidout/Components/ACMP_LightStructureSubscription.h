
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Assets/ACTR_EnergyBarrier.h"
#include "Assets/ACTR_PowerGenerator.h"
#include "Assets/ACTR_EnergyTower.h"
#include "ACMP_LightStructureSubscription.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDOUT_API UACMP_LightStructureSubscription : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	class USphereComponent* m_collisionLightZone{ nullptr };

	UPROPERTY()
	TArray<AActor*> m_enemiesSubscribed{ nullptr };

	UPROPERTY()
	TArray<AActor*> m_towerSubscribed{ nullptr };

	bool m_stopSubscribingEnemies{ false };

public:

	// Sets default values for this component's properties
	UACMP_LightStructureSubscription();

	//Current suscribed actors
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int m_suscribedActors{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower and Generator")
	TArray<AACTR_EnergyBarrier*> m_energyBarrier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	TArray<AACTR_EnergyTower*> m_towers;

	UPROPERTY()
	TArray<AACTR_PowerGenerator*> m_generator;

	//Say if tower is actived or not
	bool m_isActive{ false };

	//Getters
	inline int GetSuscribedActors() const { return m_suscribedActors; }

	inline float GetIsActive() const { return m_isActive; }

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	virtual void SubscribeEnemyToTower();

	UFUNCTION(BlueprintCallable, Category = "AISoldier")
	virtual void UnsuscribeEnemyFromTower(AActor* unsuscribedEnemy);

	void StopTower();

	UFUNCTION(BlueprintCallable, Category = "Generator")
	virtual void subscribeTowerToGenerator();

	UFUNCTION(BlueprintCallable, Category = "Generator")
	virtual void unsuscribeTowerFromGenerator();

	void resetSubscriptionsOnLoadGame();

	void StopGenerator();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	void stopSubscribingEnemies();

	//Enter in the LightZone
	UFUNCTION(Category = "Subscription")
	void OnLightZoneOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
