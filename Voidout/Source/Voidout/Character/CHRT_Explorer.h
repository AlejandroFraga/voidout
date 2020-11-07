
#pragma once

#include "CoreMinimal.h"
#include "Character/CHRT_Enemy.h"
#include "CHRT_Explorer.generated.h"

UCLASS()
class VOIDOUT_API ACHRT_Explorer : public ACHRT_Enemy
{
	GENERATED_BODY()

public:
	ACHRT_Explorer();

	/**Array of Points to follow*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomForExplorer")
	TArray<class ATGPT_Explorer*> m_path = TArray<ATGPT_Explorer*>();

	/**Projectile to shoot*/
	UPROPERTY(BlueprintReadWrite, Category = "CustomForExplorer")
	TSubclassOf<class AACTR_Flare> m_flareClass{ nullptr };

	/**The torches that creates in the path*/
	UPROPERTY(BlueprintReadWrite, Category = "CustomForExplorer")
	TSubclassOf<AActor> m_energyTorche{ nullptr };

	/**Run speed*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomForExplorer")
	float m_runSpeed{ 500.0f };

	/**Walk speed*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomForExplorer")
	float m_walkSpeed{ 300.0f };

	/**Time To Wait For Tocrh*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomForExplorer")
	float m_waitForTocrh{ 2.0f };

	/**Distance to see player*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomForExplorer")
	float m_visionConeLength{ 700.0f };

	/**Time Between Shoot*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomForExplorer")
	float m_timeBetweenShoot{ 0.6f };

	bool isDead{ false };
	
	void DisablePathComplete();

	virtual int TakeDamage(float damage) override;

	virtual void Death() override;

	void Respawn();
};
