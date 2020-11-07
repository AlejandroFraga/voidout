
#pragma once

#include "CoreMinimal.h"
#include "CHRT_AliveCharacter.h"
#include "CHRT_Player.generated.h"

UCLASS()
class VOIDOUT_API ACHRT_Player : public ACHRT_AliveCharacter
{
	GENERATED_BODY()

	/* VARIABLES */

		/* Dark Slash */

		class UParticleSystemComponent* darkSlashPSC{ nullptr };

		/* Shadow Dash */

	UParticleSystemComponent* shadowDashPSC{ nullptr };

	UParticleSystemComponent* shadowDashTrailPSC{ nullptr };

	float m_delayedDamage{ 0.f };

protected:

	/* FUNCTIONS */

	virtual void BeginPlay() override;

	/* VARIABLES */

		/* Dark Slash */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Dark Slash")
	class UParticleSystem* darkSlashPS{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Dark Slash")
	UParticleSystem* darkSlashDamagePS{ nullptr };

		/* Shadow Dash */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Shadow Dash")
	UParticleSystem* shadowDashPS{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Shadow Dash")
	UParticleSystem* shadowDashTrailPS{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Shadow Dash")
	UParticleSystem* shadowDashDamagePS{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Shadow Dash")
	class UMaterialInterface* normalMaterial{ nullptr };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Shadow Dash")
	UMaterialInterface* shadowDashMaterial{ nullptr };

		/* Voidout */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Voidout")
	class UParticleSystem* voidoutPS{ nullptr };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Voidout")
	UMaterialInterface* voidoutMaterial{ nullptr };

public:

	/* VARIABLES */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (UIMin = '0'))
	float cameraDistance{ 1000.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (UIMin = '0'))
	float cameraFOV{ 105.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (UIMin = '0'))
	float cameraRotatorX{ -70.f };

		/** Camera boom positioning the camera above the character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* MainSpringArm{ nullptr };

		/** Top down camera */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* MainCamera{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float m_timeWaitingDamage{ 0.01f };

	/* FUNCTIONS */

	ACHRT_Player();

	class ACTRL_Player* GetCTRLPlayer() const;

	class UGMIN_Base* GetGMINBase() const;

	void DelayDamage(float damage);

	void TakeDelayedDamage();

	virtual int TakeDamage(float damage) override;

	virtual void Death() override;

	UFUNCTION(BlueprintCallable, Category = "Save Game")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Save Game")
	void LoadGame();

	UFUNCTION(BlueprintImplementableEvent)
	void ResetDamageScreen();

		/* Dark Slash */

	UFUNCTION(BlueprintCallable, Category = "Abilities | Dark Slash")
	void PlayDarkSlashPS();

	UFUNCTION(BlueprintCallable, Category = "Abilities | Dark Slash")
	void PlayDarkSlashDamagePS();

		/* Shadow Dash */

	UFUNCTION(BlueprintCallable, Category = "Abilities | Shadow Dash")
	void StartShadowDashPS();

	UFUNCTION(BlueprintCallable, Category = "Abilities | Shadow Dash")
	void StopShadowDashPS();

		/* Voidout */

	UFUNCTION(BlueprintCallable, Category = "Abilities | Voidout")
	void PlayVoidoutPS(FVector location);

	UFUNCTION(BlueprintCallable, Category = "Abilities | Voidout")
	void StopVoidoutPS();

};
