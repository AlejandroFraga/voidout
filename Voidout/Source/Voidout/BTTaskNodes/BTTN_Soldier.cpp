
#include "BTTN_Soldier.h"
#include "AIController/AICT_Soldier.h"

EBTNodeResult::Type UBTTN_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* soldierController = Cast<AAICT_Soldier>(OwnerComp.GetOwner());
		if (soldierController != nullptr)
			soldierController->Shoot();
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_GetNewTower::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		AAICT_Soldier* soldierController = Cast<AAICT_Soldier>(OwnerComp.GetOwner());
		//if (soldierController != nullptr)
			//soldierController->getNewTower();
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_CheckNearbyPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bool result = false;
	if (OwnerComp.GetOwner() != nullptr)
	{
		AAICT_Soldier* soldierController = Cast<AAICT_Soldier>(OwnerComp.GetOwner());
		if (soldierController != nullptr)
			result = soldierController->CheckNearbyPlayer();
	}
	return result ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTN_GetNewPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		AAICT_Soldier* soldierController = Cast<AAICT_Soldier>(OwnerComp.GetOwner());
		if (soldierController != nullptr)
			soldierController->getNewPosition();
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_GetAttackPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		AAICT_Soldier* soldierController = Cast<AAICT_Soldier>(OwnerComp.GetOwner());
		if (soldierController != nullptr)
		soldierController->GetAttackAreaPoint();
	}
	return EBTNodeResult::Succeeded;
}
