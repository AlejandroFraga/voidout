
#pragma once

#include "CoreMinimal.h"
#include "Engine/LatentActionManager.h"
#include "SYST_DelayManager.generated.h"

UCLASS()
class VOIDOUT_API USYST_DelayManager : public UObject
{
	GENERATED_BODY()

public:

	// Constructor
	USYST_DelayManager() = default;

	// Delay function
	/*UFUNCTION(BlueprintCallable, Category = "Delay Manager")
	FLatentActionInfo Delay(UObject* WorldContextObject, UObject* InCallbackTarget, const FString& FunctionName = "", float duration = 0.2f, int32 InLinkage = -1, int32 InUUID = -1);*/

	// Delay function
	UFUNCTION(BlueprintCallable, Category = "Delay Manager")
	FLatentActionInfo RetriggerableDelay(UObject* WorldContextObject, UObject* InCallbackTarget, const FString& FunctionName = "", float duration = 0.2f, int32 InLinkage = -1, int32 InUUID = -1);

private:

	int32 m_InLinkage{ 0 };

	int32 m_InUUID{ 0 };
};
