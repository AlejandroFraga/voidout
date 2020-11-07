// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTN_Commander.h"
#include "AIController/AICT_Commander.h"

EBTNodeResult::Type UBTTN_CommanderGetNextTargetPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* owner = OwnerComp.GetOwner();
	if (!owner) return EBTNodeResult::Failed;

	auto* ctrl = Cast<AAICT_Commander>(owner);
	ctrl->GetNextPoint();

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_CommanderMoveMinion::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* owner = OwnerComp.GetOwner();
	if (!owner) return EBTNodeResult::Failed;

	auto* ctrl = Cast<AAICT_Commander>(owner);
	ctrl->MoveMinions();

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_CommanderChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* owner = OwnerComp.GetOwner();
	if (!owner) return EBTNodeResult::Failed;

	auto* ctrl = Cast<AAICT_Commander>(owner);
	ctrl->ChasePlayer();

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTN_CommanderAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* owner = OwnerComp.GetOwner();
	if (!owner) return EBTNodeResult::Failed;

	auto* ctrl = Cast<AAICT_Commander>(owner);
	ctrl->Attack(); // Attack on player.

	return EBTNodeResult::Succeeded;
}
