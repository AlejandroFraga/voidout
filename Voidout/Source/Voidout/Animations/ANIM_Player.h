
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ANIM_Player.generated.h"

UCLASS()
class VOIDOUT_API UANIM_Player : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Dark Slash")
	bool isSlashing{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Voidout")
	bool isVoidout{ false };

	UFUNCTION(BlueprintCallable, Category = "Abilities | Dark Slash")
	void SetIsSlashing(bool _isSlashing = true) { isSlashing = _isSlashing; };

	UFUNCTION(BlueprintCallable, Category = "Abilities | Voidout")
	void SetIsVoidout(bool _isVoidout = true) { isVoidout = _isVoidout; };
};
