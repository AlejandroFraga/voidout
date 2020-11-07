
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Assets/ACTR_DestroyableLight.h"
#include "Character/CHRT_Explorer.h"
#include "ACTR_EnergyTorche.generated.h"

UCLASS()
class VOIDOUT_API AACTR_EnergyTorche : public AACTR_DestroyableLight
{
	GENERATED_BODY()
	
public:

	AACTR_EnergyTorche();

	class ACHRT_Explorer* m_explorerOwner{ nullptr };

	void UpdateTargetPoint() const;

	virtual bool Death() override;

};
