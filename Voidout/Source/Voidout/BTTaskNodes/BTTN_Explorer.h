
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTN_Explorer.generated.h"

/**
 * Shoot the flare
 */
UCLASS()
class VOIDOUT_API UBTTN_ShootFlare : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Shoot the flare to the player"); }
};

/**
 * When shooting the player, rotate towards him
 */
UCLASS()
class VOIDOUT_API UBTTN_RotateTowardsPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Rotate looking at player"); }
};


/**
 * Get the next point in the path
 */
UCLASS()
class VOIDOUT_API UBTTN_GetNextTargetPoint : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Update the next point in the path"); }
};

/**
 * Spawn a lamp post
 */
UCLASS()
class VOIDOUT_API UBTTN_CreateLight : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Create lamp post"); }
};

/**
 * Change the bool running from player
 */
UCLASS()
class VOIDOUT_API UBTTN_ChangeRunningState : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Change the boolean running away from player"); }
};