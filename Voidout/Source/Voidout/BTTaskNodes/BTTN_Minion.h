
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTN_Minion.generated.h"

/**
 * When shooting the player, rotate towards him
 */
UCLASS()
class VOIDOUT_API UBTTN_MinionFocus : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Focus on player player"); }
};

/**
 * After shooting the player, stop rotating towards him
 */
UCLASS()
class VOIDOUT_API UBTTN_MinionResetFocus : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Reset focus"); }
};

/**
 * Check if player is near to minimun distance
 */
UCLASS()
class VOIDOUT_API UBTTN_MinionCheckNearbyPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Check if player is near to minimun distance"); }
};

/**
 * Shoot player.
 */
UCLASS()
class VOIDOUT_API UBTTN_MinionShoot : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Minion's Shoot"); }
};

/**
 * Move.
 */
UCLASS()
class VOIDOUT_API UBTTN_MinionMove : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Shoot"); }
};