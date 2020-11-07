
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACTR_EnergyBarrier.generated.h"

UCLASS()
class VOIDOUT_API AACTR_EnergyBarrier : public AActor
{
	GENERATED_BODY()

	void ChangeWallStatus(bool status);

	bool IsCHRTPlayer(AActor* otherActor, UActorComponent* otherComp) const;

	class ACHRT_Player* GetCHRTPlayer() const;

	void InitialDamage(ACHRT_Player* player) const;

	void DamageInside(ACHRT_Player* player, float deltaTime) const;

	void InvertVelocity(ACHRT_Player* player);

	class UStaticMeshComponent* m_onBarrier{ nullptr };

	class UStaticMeshComponent* m_offBarrier{ nullptr };

public:	

	//Wall Box Collider 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* m_collisionComponent{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float repulsionDamage{ 10.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float insideDamageMultiplier{ 2.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float timeToUnlock{ .5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInstance* m_energyBarrierLockedMaterial{ nullptr };

	TArray<AActor*> m_childActors = TArray<AActor*>();

	//Current suscribed actors
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int m_subscribedActors{ 0 };

	//Say is Wall is actived or not
	bool m_isActive{ false };

	// Sets default values for this actor's properties
	AACTR_EnergyBarrier();

	UFUNCTION(BlueprintCallable)
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Getters
	inline int GetSuscribedActors() const { return m_subscribedActors; }
	inline float GetIsActive() const { return m_isActive; }
	inline UBoxComponent* getBoxComponent()   const noexcept { return m_collisionComponent; }

	UFUNCTION(BlueprintCallable)
	virtual void SubscribeToWall();

	UFUNCTION(BlueprintCallable)
	virtual void UnsubscribeToWall();

	void resetEnergyBarrierOnLoadGame();//Used to initializate all the variables when the player is death.

	UFUNCTION(BlueprintCallable)
	virtual void UnLockWall();

	UFUNCTION(BlueprintCallable)
	virtual void LockWall();

	void LockWallOnFinishLevel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_isForStopMusic{ false };

protected:

	UPROPERTY(BlueprintReadWrite)
	bool isPlayerInside{ false };

	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ForceFeedbackHit();

};
