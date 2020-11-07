// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ANIM_Commander.generated.h"

/**
 * 
 */
UCLASS()
class VOIDOUT_API UANIM_Commander : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Abilities | Dark Slash")
	bool m_isAttacking{ false };

	UFUNCTION(BlueprintCallable, Category = "Abilities | Principal Attack")
	void SetIsAttacking(bool isAttacking) noexcept { m_isAttacking = isAttacking; };
};

