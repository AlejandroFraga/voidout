
#pragma once

#include "CoreMinimal.h"
#include "Assets/ACTR_DestroyableLight.h"
#include "ACTR_PowerGenerator.generated.h"

UCLASS()
class VOIDOUT_API AACTR_PowerGenerator : public AACTR_DestroyableLight
{
	GENERATED_BODY()

	void CantDestroy();

public:

	AACTR_PowerGenerator();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Can't destroy")
	float repulsionDamage{ 10.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Can't destroy")
	int repulsionForce{ 6000 };

	virtual bool Death() override;

};
