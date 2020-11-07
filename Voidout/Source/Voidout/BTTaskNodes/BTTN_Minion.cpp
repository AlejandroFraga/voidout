
#include "BTTN_Minion.h"
#include "AIController/AICT_Minion.h"


// --------------------------------------------------------------------------------------------------------------------- UBTTN_MinionFocus
EBTNodeResult::Type UBTTN_MinionFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* ctrl = Cast<AAICT_Minion>(OwnerComp.GetOwner());
		if (ctrl != nullptr)
			ctrl->Focus();
	}
	return EBTNodeResult::Succeeded;
}

// --------------------------------------------------------------------------------------------------------------------- UBTTN_MinionFocus
EBTNodeResult::Type UBTTN_MinionResetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* ctrl = Cast<AAICT_Minion>(OwnerComp.GetOwner());
		if (ctrl != nullptr)
			ctrl->ResetFocus();
	}
	return EBTNodeResult::Succeeded;
}

// --------------------------------------------------------------------------------------------------------------------- UBTTN_MinionCheckNearbyPlayer
EBTNodeResult::Type UBTTN_MinionCheckNearbyPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bool result = false;
	if (OwnerComp.GetOwner() != nullptr)
	{
		const auto* ctrl = Cast<AAICT_Minion>(OwnerComp.GetOwner());
		if (ctrl != nullptr)
			result = ctrl->CheckNearbyPlayer();
	}
	return result ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

// --------------------------------------------------------------------------------------------------------------------- UBTTN_MinionShoot
EBTNodeResult::Type UBTTN_MinionShoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* ctrl = Cast<AAICT_Minion>(OwnerComp.GetOwner());
		if (ctrl != nullptr)
			ctrl->Shoot();
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_MinionShoot::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		// When abort, just look at the player position (can be changed).
		auto* ctrl = Cast<AAICT_Minion>(OwnerComp.GetOwner());
		if (ctrl != nullptr)
			ctrl->Focus();
	}
	return EBTNodeResult::Succeeded;
}

// --------------------------------------------------------------------------------------------------------------------- UBTTN_MinionMove
EBTNodeResult::Type UBTTN_MinionMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* ctrl = Cast<AAICT_Minion>(OwnerComp.GetOwner());
		if (ctrl != nullptr)
			ctrl->Move();
	}
	return EBTNodeResult::Succeeded;
}
