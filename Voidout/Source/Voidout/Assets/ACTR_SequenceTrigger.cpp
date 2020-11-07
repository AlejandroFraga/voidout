
#include "ACTR_SequenceTrigger.h"
#include "Character/CHRT_Player.h"
#include "Controller/CTRL_Player.h"
#include "GameInstance/GMIN_Base.h"
#include "System/SYST_DelayManager.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SystemsManager.h"
#include "UserWidget/USWG_MenuBase.h"

#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MovieSceneSequencePlayer.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "TimeManagementBlueprintLibrary.h"
#include "UObject/ConstructorHelpers.h"


AACTR_SequenceTrigger::AACTR_SequenceTrigger()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> USWG_SkipSequence_Class(TEXT("/Game/Blueprints/UserWidget/HUD/USWG_SkipSequence"));
	skipSequenceClass = USWG_SkipSequence_Class.Class;

	static ConstructorHelpers::FClassFinder<UUSWG_MenuBase> USWG_Credits_Class(TEXT("/Game/Blueprints/UserWidget/Credits/USWG_Credits"));
	creditsClass = USWG_Credits_Class.Class;
}

void AACTR_SequenceTrigger::BeginPlay()
{
	if (GetCollisionComponent() == nullptr)
		return;

	GetCollisionComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &AACTR_SequenceTrigger::OnOverlapBegin);

	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AACTR_SequenceTrigger::OnOverlapBegin);

	// We hide the actors that are only for the sequence
	ShowActorsInSequence(false);
}

void AACTR_SequenceTrigger::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsActorPlayerAndSequenceNotPlayed(OtherActor, OtherComp))
	{
		PlayTheLevelSequence();
	}
}

ACHRT_Player* AACTR_SequenceTrigger::GetCHRTPlayer()
{
	if (player == nullptr)
	{
		player = Cast<ACHRT_Player>(UGameplayStatics::GetPlayerPawn(this, 0));
	}
	return player;
}

ACTRL_Player* AACTR_SequenceTrigger::GetCTRLPlayer()
{
	if (playerController == nullptr)
	{
		playerController = Cast<ACTRL_Player>(UGameplayStatics::GetPlayerController(this, 0));
	}
	return playerController;
}

UGMIN_Base* AACTR_SequenceTrigger::GetGameInstance()
{
	return Cast<UGMIN_Base>(UGameplayStatics::GetGameInstance(GetWorld()));
}

bool AACTR_SequenceTrigger::IsActorPlayer(AActor* otherActor, UActorComponent* otherComponent)
{
	return Cast<ACHRT_Player>(otherActor) != nullptr && Cast<UCapsuleComponent>(otherComponent) != nullptr;
}

bool AACTR_SequenceTrigger::IsActorPlayerAndSequenceNotPlayed(AActor* otherActor, UActorComponent* otherComponent)
{
	return IsActorPlayer(otherActor, otherComponent) && !played;
}

void AACTR_SequenceTrigger::PrepareToSequence(bool hideThePlayer)
{
	// Prepare to the sequence disabling the inputs and cleaning HUD elements from the screen
	SetStaticMobility();
	HideActorsInSequence(hideThePlayer);
	ShowActorsInSequence();
	PrepareToSequenceView();
	SetSequenceGMIN();
}

void AACTR_SequenceTrigger::ReturnFromSequence()
{
	// Prepare to the sequence disabling the inputs and cleaning HUD elements from the screen
	SetSequenceGMIN(false);
	ReturnFromSequenceView();
	ShowActorsInSequence(false);
	HideActorsInSequence(true, false);
	SetStaticMobility(false);
}

void AACTR_SequenceTrigger::SetStaticMobility(bool set)
{
	if (GetCHRTPlayer() != nullptr)
	{
		UCapsuleComponent* playerCapsule = Cast<UCapsuleComponent>(GetCHRTPlayer()->GetComponentByClass(UCapsuleComponent::StaticClass()));
		if (playerCapsule != nullptr)
			playerCapsule->SetMobility(set ? EComponentMobility::Stationary : EComponentMobility::Movable);
		else
			USYST_LogManager::PrintError(this, "AACTR_SequenceTrigger.SetSequenceCTRL: No CHRTPlayer->UCapsuleComponent");
	}
	else
		USYST_LogManager::PrintError(this, "AACTR_SequenceTrigger.SetSequenceCTRL: No CHRTPlayer");
}

void AACTR_SequenceTrigger::SetSequenceGMIN(bool set)
{
	if (GetGameInstance() != nullptr)
	{
		// We set the sequenceTrigger of the Player Controller to this
		GetGameInstance()->sequenceTrigger = set ? this : nullptr;

		// We change the player status
		GetGameInstance()->playerStatus = set ? EPlayerStatus::InSequence : EPlayerStatus::InGame;
	}
	else
		USYST_LogManager::PrintError(this, "AACTR_SequenceTrigger.SetSequenceCTRL: No GameInstance");
}

void AACTR_SequenceTrigger::PrepareToSequenceView()
{
	if (GetCTRLPlayer() != nullptr)
	{
		//FIXME se puede mejorar con SYST_MenuManager
		// Hide the elements from the screen, and remember which were showing
		wasShowingHUDBase = wasShowingHUDBase || GetCTRLPlayer()->ShowHUDBase(false);
		wasShowingTutorialMenu = wasShowingTutorialMenu || GetCTRLPlayer()->ShowTutorial(false);
		wasShowingShadowDashIndicator = wasShowingShadowDashIndicator || GetCTRLPlayer()->SetShadowDashIndicatorShow(false);
		wasShowingTheMouse = wasShowingTheMouse || GetCTRLPlayer()->ShowMouseCursor(false);

		if (canBeSkiped)
		{
			skipSequence = CreateWidget<UUserWidget>(GetCTRLPlayer(), skipSequenceClass);
			if (skipSequence != nullptr)
			{
				skipSequence->AddToViewport(999);
				skipSequence->SetVisibility(ESlateVisibility::Hidden);
			}

			setCanBeSkipedTimer = SYST_SystemsManager::GetDelayManager()->RetriggerableDelay(this, this, "SetCanSkip", timeUntilCanSkipSequence);
		}
	}
	else
		USYST_LogManager::PrintError(this, "AACTR_SequenceTrigger.SetSequenceCTRL: No CTRLPlayer");
}

void AACTR_SequenceTrigger::ReturnFromSequenceView()
{
	if (GetCTRLPlayer() != nullptr)
	{
		// Show back the elements that were showed before the sequence played
		//FIXME se puede mejorar con SYST_MenuManager
		GetCTRLPlayer()->ShowHUDBase(wasShowingHUDBase);
		GetCTRLPlayer()->ShowTutorial(wasShowingTutorialMenu);
		GetCTRLPlayer()->SetShadowDashIndicatorShow(wasShowingShadowDashIndicator);
		GetCTRLPlayer()->ShowMouseCursor(wasShowingTheMouse);
		StopMusicSequence();

		if (skipSequence != nullptr)
			skipSequence->RemoveFromParent();

		if(isFinalSequence)
		{
			credits = CreateWidget<UUSWG_MenuBase>(GetCTRLPlayer(), creditsClass);
			if (credits != nullptr)
			{
				credits->AddToViewport(1000);
				GetCTRLPlayer()->activeMenu = credits;
			}
		}
	}
	else
		USYST_LogManager::PrintError(this, "AACTR_SequenceTrigger.SetSequenceCTRL: No CTRLPlayer");
}

void AACTR_SequenceTrigger::HideActorsInSequence(bool applyToPlayer, bool hide)
{
	if (actorsToHideInSequence.Num() > 0)
	{
		for (int i = 0; i < actorsToHideInSequence.Num(); i++)
		{
			if (actorsToShowInSequence[i] != nullptr)
			{
				actorsToHideInSequence[i]->SetActorHiddenInGame(hide);
				actorsToHideInSequence[i]->SetActorEnableCollision(!hide);
			}
		}
		if (GetCHRTPlayer() != nullptr && applyToPlayer)
		{
			GetCHRTPlayer()->SetActorHiddenInGame(hide);
			GetCHRTPlayer()->SetActorEnableCollision(!hide);
		}
	}
}

void AACTR_SequenceTrigger::ShowActorsInSequence(bool show)
{
	if (actorsToShowInSequence.Num() > 0)
	{
		for (int i = 0; i < actorsToShowInSequence.Num(); i++)
		{
			if(actorsToShowInSequence[i] != nullptr)
			{
				actorsToShowInSequence[i]->SetActorHiddenInGame(!show);
				actorsToShowInSequence[i]->SetActorEnableCollision(show);

				if (!show && played)
					actorsToShowInSequence[i]->Destroy();
			}
		}
	}
}

float AACTR_SequenceTrigger::ReproduceTheLevelSequence()
{
	if (levelSequence != nullptr)
	{
		// Create a level sequence player and play the level sequence
		ALevelSequenceActor* OutActor;
		FMovieSceneSequencePlaybackSettings Settings;
		levelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), levelSequence, Settings, OutActor);
		if(levelSequencePlayer != nullptr)
			levelSequencePlayer->Play();

		SYST_SystemsManager::GetSoundEventManager()->stopFightMusic();
		StopMusicSequence();
		StartMusicSequence();
		SYST_SystemsManager::GetSoundEventManager()->playMusicBus();

		// Returns the duration of the level sequence converted to seconds
		return UTimeManagementBlueprintLibrary::Conv_QualifiedFrameTimeToSeconds(levelSequencePlayer->GetDuration());
	}
	else
		USYST_LogManager::PrintError(this, "AACTR_SequenceTrigger.SetSequenceCTRL: No levelSequence");
	return 0.f;
}

void AACTR_SequenceTrigger::PlayTheLevelSequence(bool hideThePlayer)
{
	if (levelSequence != nullptr)
	{
		// Prepare to play the sequence
		PrepareToSequence();
	
		// Play the level sequence and get the duration
		float duration = ReproduceTheLevelSequence();

		// After the duration of the sequence, return from it
		returnFromSequenceTimer = SYST_SystemsManager::GetDelayManager()->RetriggerableDelay(this, this, "ReturnFromSequence", duration);

		// Mark the sequence as played
		played = true;
	}
	else
		USYST_LogManager::PrintWarning(this, "AACTR_SequenceTrigger.SetSequenceCTRL: No levelSequence");
}

void AACTR_SequenceTrigger::SetCanSkip()
{
	if (canBeSkiped)
	{
		canSkip = true;

		if (skipSequence != nullptr)
			skipSequence->SetVisibility(ESlateVisibility::Visible);
	}
}

void AACTR_SequenceTrigger::SkipTheLevelSequence()
{
	if (levelSequencePlayer != nullptr)
	{
		// End the sequence and change delay to 0, so it'll be executed inmediatly
		levelSequencePlayer->GoToEndAndStop();
		//StopMusicSequence();
		setCanBeSkipedTimer = SYST_SystemsManager::GetDelayManager()->RetriggerableDelay(this, this, "SetCanSkip", 0.f, setCanBeSkipedTimer.Linkage, setCanBeSkipedTimer.UUID);
		returnFromSequenceTimer = SYST_SystemsManager::GetDelayManager()->RetriggerableDelay(this, this, "ReturnFromSequence", 0.f, returnFromSequenceTimer.Linkage, returnFromSequenceTimer.UUID);
	}
}