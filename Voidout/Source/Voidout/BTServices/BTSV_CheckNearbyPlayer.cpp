
#include "BTSV_CheckNearbyPlayer.h"
#include "AIController/AICT_Explorer.h"


void UBTSV_CheckNearbyPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* explorerAIController = Cast<AAICT_Explorer>(OwnerComp.GetOwner());
		if (explorerAIController != nullptr)
			explorerAIController->CheckNearbyEnemy();
	}
}
