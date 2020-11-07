
#pragma once

#include "CoreMinimal.h"
#include "Engine/LatentActionManager.h"
#include "Engine/TriggerBox.h"
#include "ACTR_SequenceTrigger.generated.h"

UCLASS()
class VOIDOUT_API AACTR_SequenceTrigger : public ATriggerBox
{
	GENERATED_BODY()

	// ---------------------------------------------------- Variables.

	/* COMMON */

	FLatentActionInfo returnFromSequenceTimer = FLatentActionInfo();

	FLatentActionInfo setCanBeSkipedTimer = FLatentActionInfo();

	UPROPERTY() class ULevelSequencePlayer* levelSequencePlayer{ nullptr };

	UPROPERTY() class ACHRT_Player* player{ nullptr };

	UPROPERTY() class ACTRL_Player* playerController{ nullptr };

	UPROPERTY() bool played{ false };

	//FIXME se puede mejorar con SYST_MenuManager
	bool wasShowingHUDBase{ false };

	bool wasShowingTutorialMenu{ false };

	bool wasShowingShadowDashIndicator{ false };

	bool wasShowingTheMouse{ false };

	TSubclassOf<class UUserWidget> skipSequenceClass{ nullptr };

	UUserWidget* skipSequence{ nullptr };

	TSubclassOf<class UUSWG_MenuBase> creditsClass{ nullptr };

	UUSWG_MenuBase* credits{ nullptr };

	// ---------------------------------------------------- Methods.

	/* COMMON */

	ACHRT_Player* GetCHRTPlayer();

	ACTRL_Player* GetCTRLPlayer();

	class UGMIN_Base* GetGameInstance();

	bool IsActorPlayer(AActor* otherActor, UActorComponent* otherComponent);

	void PrepareToSequence(bool hideThePlayer = true);

	UFUNCTION()
	void ReturnFromSequence();

	void SetStaticMobility(bool set = true);

	void SetSequenceGMIN(bool set = true);

	void PrepareToSequenceView();

	void ReturnFromSequenceView();

	void HideActorsInSequence(bool aplyToPlayer = true, bool hide = true);

	void ShowActorsInSequence(bool show = true);

	float ReproduceTheLevelSequence();

protected:

	virtual void BeginPlay() override;
	
public:

	// ---------------------------------------------------- Variables.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence", meta = (UIMin = '0'))
	float timeUntilCanSkipSequence{ 3.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence", meta = (UIMin = '0'))
	float timeHoldingToSkipSequence{ 2.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool canBeSkiped{ true };

	UPROPERTY()
	bool canSkip{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool isFinalSequence{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	class ULevelSequence* levelSequence{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	TArray<AActor*> actorsToHideInSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	TArray<AActor*> actorsToShowInSequence;

	// ---------------------------------------------------- Methods.

	AACTR_SequenceTrigger();

	UFUNCTION(BlueprintCallable, Category = "Collision")
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	bool IsActorPlayerAndSequenceNotPlayed(AActor* otherActor, UActorComponent* otherComponent);

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void PlayTheLevelSequence(bool hideThePlayer = true);

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void SetCanSkip();

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void SkipTheLevelSequence();

	//FMOD Functions
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Music Sequence Control")
		void StartMusicSequence();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Music Sequence Control")
		void StopMusicSequence();

};
