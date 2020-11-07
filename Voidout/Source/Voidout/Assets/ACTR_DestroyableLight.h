
#pragma once

#include "CoreMinimal.h"
#include "Assets/ACTR_Light.h"
#include "ACTR_DestroyableLight.generated.h"

UCLASS()
class VOIDOUT_API AACTR_DestroyableLight : public AACTR_Light
{
	GENERATED_BODY()
	
public:

	// Health component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	class UACMP_Health* m_health{ nullptr };

	// Sets default values for this actor's properties
	AACTR_DestroyableLight();

	virtual bool Death();

};
