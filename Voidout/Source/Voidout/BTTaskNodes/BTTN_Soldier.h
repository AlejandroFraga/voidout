
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTN_Soldier.generated.h"

/**
*Shoot player
*/
UCLASS()
class VOIDOUT_API UBTTN_Shoot : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Soldier's Shoot."); }
};

/**
*Find Nearest Tower
*/
UCLASS()
class VOIDOUT_API UBTTN_GetNewTower : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Look For Nearest Tower"); }

};

/**
 * Check if player is near to minimun distance
 */
UCLASS()
class VOIDOUT_API UBTTN_CheckNearbyPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Check if player is near to minimun distance"); }
};

/**
 * Soldier Patrol Movement
 */
UCLASS()
class VOIDOUT_API UBTTN_GetNewPosition : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Get new position inside tower"); }
};

/**
 * Soldier Attack Movement
 */
UCLASS()
class VOIDOUT_API UBTTN_GetAttackPosition : public UBTTaskNode
{
	GENERATED_BODY()
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	inline FString GetStaticDescription() const noexcept override { return TEXT("Get new position inside safe area to attack"); }
};