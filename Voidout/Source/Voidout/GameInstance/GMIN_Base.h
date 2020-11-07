
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FMODStudioModule.h"
#include "GMIN_Base.generated.h"

UENUM()
enum EPlayerStatus
{
	InMenu      UMETA(DisplayName = "InMenu"),
	InGame      UMETA(DisplayName = "InGame"),
	InSequence  UMETA(DisplayName = "InSequence"),
};

UENUM()
enum EController
{
	PlayStation     UMETA(DisplayName = "PlayStation"),
	Xbox			UMETA(DisplayName = "Xbox"),
	Pc				UMETA(DisplayName = "Pc"),
};

UCLASS()
class VOIDOUT_API UGMIN_Base : public UGameInstance
{
	GENERATED_BODY()

	/* VARIABLES */

	TSubclassOf<class ASYST_SoundManager> soundManagerClass{ nullptr };

	TSubclassOf<class ASYST_SoundEventManager> soundEventManagerClass{ nullptr };

public:

	/* VARIABLES */
	
	UPROPERTY(BlueprintReadOnly, Category = Status)
	TEnumAsByte<EPlayerStatus> playerStatus = InGame;

	UPROPERTY(BlueprintReadOnly, Category = "Controls")
	TEnumAsByte<EController> controllerType = PlayStation;

	UPROPERTY(BlueprintReadOnly, Category = "Controls")
	bool isUsingController{ false };

	UPROPERTY(BlueprintReadWrite, Category = "Controls")
	bool isVibrationEnabled{ true };

	UPROPERTY(BlueprintReadWrite, Category = "Controls")
	float brightnessValue{ 1.f };

	UPROPERTY(BlueprintReadOnly, Category = "Level Sequence")
	class AACTR_SequenceTrigger* sequenceTrigger{ nullptr };


	/* FUNCTIONS */

	UGMIN_Base();

	UFUNCTION(BlueprintCallable, Category = "Common")
	void UpdateSystemManager();

	UFUNCTION(BlueprintCallable, Category = "Sound Event Manager")
	ASYST_SoundEventManager* GetSoundEventManager();

	//FIXME Until Menu Manager
	UFUNCTION(BlueprintCallable, Category = "Menu | Tutorial Menu")
	bool ShowTutorial(bool show = true, bool pause = true, bool setActive = true);

};