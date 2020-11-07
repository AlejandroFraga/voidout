
#include "BTTN_Explorer.h"
#include "AIController/AICT_Explorer.h"


EBTNodeResult::Type UBTTN_ShootFlare::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* explorerController = Cast<AAICT_Explorer>(OwnerComp.GetOwner());
		if (explorerController != nullptr)
			explorerController->ShootFlare();
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_RotateTowardsPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* explorerController = Cast<AAICT_Explorer>(OwnerComp.GetOwner());
		if (explorerController != nullptr)
			explorerController->RotateTowardsPlayer();
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_GetNextTargetPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* explorerController = Cast<AAICT_Explorer>(OwnerComp.GetOwner());
		if (explorerController != nullptr)
			explorerController->GetNextTargetPoint();
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_CreateLight::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* explorerController = Cast<AAICT_Explorer>(OwnerComp.GetOwner());
		if (explorerController != nullptr)
			explorerController->CreateLight();
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_ChangeRunningState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetOwner() != nullptr)
	{
		auto* explorerController = Cast<AAICT_Explorer>(OwnerComp.GetOwner());
		if (explorerController != nullptr)
			explorerController->ChangeRunningState();
	}
	return EBTNodeResult::Succeeded;
}