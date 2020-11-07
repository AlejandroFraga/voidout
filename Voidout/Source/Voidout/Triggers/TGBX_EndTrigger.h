
#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TGBX_EndTrigger.generated.h"

UCLASS()
class VOIDOUT_API ATGBX_EndTrigger : public ATriggerBox
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Sequence")
	class AACTR_SequenceTrigger* endingSequenceTrigger{ nullptr };

	UFUNCTION(BlueprintCallable)
	void End();

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool canSequenceBeSkiped{ true };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	class ULevelSequence* levelSequence{ nullptr };

};
