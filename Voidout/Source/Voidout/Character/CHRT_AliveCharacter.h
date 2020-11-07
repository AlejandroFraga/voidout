
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CHRT_AliveCharacter.generated.h"

UCLASS()
class VOIDOUT_API ACHRT_AliveCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACHRT_AliveCharacter();

	// Health component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UACMP_Health* m_health{ nullptr };

	bool IsDying();

	void SetCanRecover(bool canRecover);

	virtual void LifeRecover(float m_health);

	virtual int TakeDamage(float damage);

	UFUNCTION(BlueprintCallable)
	virtual void Kill();

	virtual void Death();
};
