
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACTR_SoldierBullet.generated.h"

UCLASS()
class VOIDOUT_API AACTR_SoldierBullet : public AActor
{
	GENERATED_BODY()

	// Particles system.
	class UParticleSystem* m_particles{ nullptr };

public:

	// Bullet speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	float m_speed{ 1000.f };

	//Bullet damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	float m_damage{ 20.f };

	// Collision component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	class UCapsuleComponent* m_collisionComp{ nullptr };

	// Movement component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	class UProjectileMovementComponent* m_projMovement{ nullptr };

	// Sets default values for this actor's properties
	AACTR_SoldierBullet();

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
