
#pragma once

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACTR_Flare.generated.h"

struct FHitResult;

UCLASS()
class VOIDOUT_API AACTR_Flare : public AActor
{
	GENERATED_BODY()

	bool hitted{ false };

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Hitted")
	void PlayHittedSound();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Hitted")
	void PlayHittedNPS();

public:

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* m_collisionComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flare")
	float m_timeToMarkPlayer = 5.0;
	// Bullet speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flare")
	float m_speed{ 3000.f };

	float m_gravity = 9800.0f;

	FVector m_velocity = FVector::ZeroVector;
	

	AACTR_Flare();

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnHitted();

	void Clean();

};
