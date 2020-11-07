
#include "TGBX_EndTrigger.h"
#include "Assets/ACTR_SequenceTrigger.h"
#include "Controller/CTRL_Player.h"
#include "UserWidget/USWG_MenuBase.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


void ATGBX_EndTrigger::End()
{
	if (GetWorld() == nullptr)
		return;

	FActorSpawnParameters SpawnParameters;
	endingSequenceTrigger = GetWorld()->SpawnActor<AACTR_SequenceTrigger>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);

	if (levelSequence != nullptr && endingSequenceTrigger != nullptr)
	{
		endingSequenceTrigger->canBeSkiped = canSequenceBeSkiped;
		endingSequenceTrigger->levelSequence = levelSequence;
		endingSequenceTrigger->isFinalSequence = true;
		endingSequenceTrigger->PlayTheLevelSequence();
	}
}