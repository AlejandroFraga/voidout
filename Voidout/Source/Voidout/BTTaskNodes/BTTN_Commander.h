// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTN_Commander.generated.h"

/**
 * Get next move position.
 */
UCLASS()
class VOIDOUT_API UBTTN_CommanderGetNextTargetPoint : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Get next target point."); }
};

/**
 * Move minions.
 */
UCLASS()
class VOIDOUT_API UBTTN_CommanderMoveMinion : public UBTTaskNode
{
	GENERATED_BODY()
		EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Move minions."); }
};

/**
 * chase the player.
 */
UCLASS()
class VOIDOUT_API UBTTN_CommanderChasePlayer : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Chase the player."); }
};

/**
 * Attack to player.
 */
UCLASS()
class VOIDOUT_API UBTTN_CommanderAttack : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Attack."); }
};