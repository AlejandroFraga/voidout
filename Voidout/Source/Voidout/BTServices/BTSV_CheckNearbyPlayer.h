
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTSV_CheckNearbyPlayer.generated.h"

UCLASS()
class VOIDOUT_API UBTSV_CheckNearbyPlayer : public UBTService
{
	GENERATED_BODY()
	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
