
#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "TGPT_Explorer.generated.h"

UCLASS()
class VOIDOUT_API ATGPT_Explorer : public ATargetPoint
{
	GENERATED_BODY()

public:

	// ---------------------------------------------------- Variables.

	//ROOT
	UPROPERTY(BlueprintReadOnly, Category = "CustomForExplorer")
	USceneComponent* Root;

	//Check if is inside light
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomForExplorer")
	bool m_isInsideLight{ false };

	//Check if have lamp constructed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomForExplorer")
	bool m_haveLampPost{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomForExplorer")
	class AACTR_EnergyTorche* m_energyTorche{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomForExplorer")
	class AACTR_EnergyTower* m_energyTower{ nullptr };

	// ---------------------------------------------------- Methods.
	
	ATGPT_Explorer();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CustomForExplorer")
	class AACTR_Light* GetTorcheOrTower();
};
