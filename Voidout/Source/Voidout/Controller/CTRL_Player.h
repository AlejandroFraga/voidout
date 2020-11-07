
#pragma once

#include <memory>

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "System/SYST_Teleporter.h"
#include "CTRL_Player.generated.h"

UCLASS()
class VOIDOUT_API ACTRL_Player : public APlayerController
{
	GENERATED_BODY()

	// Teleport list.
	std::unique_ptr<SYST_Teleporter> m_teleporter{ nullptr };

	FKey skipKey{ EKeys::Invalid };

	FKey rightKey{ EKeys::Invalid };

	int rightKeyHeldCounter{ -1 };

	FKey leftKey{ EKeys::Invalid };

	int leftKeyHeldCounter{ -1 };

protected:

	/* VARIABLES */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Shadow Dash")
	bool isShadowDashUnlocked{ true };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Voidout")
	bool isVoidoutUnlocked{ true };

	/* FUNCTIONS */

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Abilities | Shadow Dash")
	void ForceFeedbackShadowDashKill();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Abilities | Voidout")
	void ForceFeedbackVoidoutReady();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Abilities | Voidout")
	void ForceFeedbackVoidoutExplosion();

public:

	/* VARIABLES */

	UPROPERTY(EditAnywhere, Category = "Abilities | Dark Slash")
	bool darkSlashDisablesControls{ true };

	UPROPERTY(EditAnywhere, Category = "Abilities | Dark Slash")
	bool darkSlashKillGivesHealth{ true };

	UPROPERTY(EditAnywhere, Category = "Abilities | Dark Slash")
	float darkSlashKillHealth{ 20.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Shadow Dash", meta = (UIMin = '0'))
	float shadowDashDistance{ 5500.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Shadow Dash", meta = (UIMin = '0'))
	float shadowDashDuration{ 0.20f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Shadow Dash", meta = (UIMin = '0'))
	float shadowDashCooldown{ 2.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Shadow Dash")
	bool isDirectionalShadowDashUnlocked{ true };

	UPROPERTY(EditAnywhere, Category = "Abilities | Shadow Dash")
	bool shadowDashKillGivesHealth{ true };

	UPROPERTY(EditAnywhere, Category = "Abilities | Shadow Dash")
	float shadowDashKillHealth{ 20.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Voidout", meta = (UIMin = '0'))
	float voidoutCooldown{ 10.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Voidout", meta = (UIMin = '0'))
	float voidoutCooldownReductionOnKill{ 2.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Voidout", meta = (UIMin = '0'))
	float voidoutEffectDuration{ 3.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities | Voidout", meta = (UIMin = '0'))
	int voidoutEnemyImpulse{ 7500 };

	UPROPERTY(BlueprintReadWrite, Category = "Menu | Active Menu")
	UUSWG_MenuBase* activeMenu{ nullptr };

	// Restarted
	UPROPERTY(BlueprintReadWrite, Category = "Menu | Pause Menu")
	bool m_restarted{ false };
	
	UPROPERTY(BlueprintReadWrite, Category = "Menu | Pause Menu")
	class UUSWG_MenuBase* pauseMenu{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadow Dash")
	UUSWG_MenuBase* startMenu{ nullptr };

	UPROPERTY(BlueprintReadWrite, Category = "Abilities | Dark Slash")
	class UWidgetComponent* darkSlashIndicator{ nullptr };

	UPROPERTY(BlueprintReadWrite, Category = "Debuffs | Marked by flare")
	UWidgetComponent* markedByFlareIndicator{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadow Dash")
	UUserWidget* shadowDashIndicator{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	float timeUntilKeyHeld{ 0.3f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	float timeBetweenKeyHeldTicks{ 0.1f };

	/* FUNCTIONS */

	ACTRL_Player();

	UFUNCTION(BlueprintCallable, Category = "General")
	void OnStartGame();

	UFUNCTION(BlueprintCallable, Category = "General")
	void ResetState();

		/* Dark Slash */

	UFUNCTION(BlueprintCallable, Category = "Abilities | Dark Slash")
	void OnSphereBeginOverlapDarkSlash(AActor* otherActor, class UActorComponent* otherComp);

	UFUNCTION(BlueprintCallable, Category = "Abilities | Dark Slash")
	void OnSphereEndOverlapDarkSlash(AActor* otherActor, UActorComponent* otherComp);

	UFUNCTION(BlueprintCallable, Category = "Abilities | Dark Slash")
	bool DarkSlashDamage();

	UFUNCTION(BlueprintCallable, Category = "Abilities | Dark Slash")
	void DarkSlashEnded();

		/* Shadow Dash */

	UFUNCTION(BlueprintCallable, Category = "Abilities | Shadow Dash")
	void UnlockShadowDash();

	UFUNCTION(BlueprintCallable, Category = "Abilities | Shadow Dash")
	void OnSphereBeginOverlapShadowDash(AActor* otherActor, UActorComponent* otherComp);

	UFUNCTION(BlueprintCallable, Category = "Abilities | Shadow Dash")
	void OnSphereEndOverlapShadowDash(AActor* otherActor, UActorComponent* otherComp);

	UFUNCTION(BlueprintCallable, Category = "Abilities | Shadow Dash")
	void UnlockVoidout();

	UFUNCTION(BlueprintCallable, Category = "Abilities | Shadow Dash")
	bool SetShadowDashIndicatorShow(const bool show = true);

		/* Voidout */

	UFUNCTION(BlueprintCallable, Category = "Abilities | Voidout")
	void OnTowerBeginOverlapVoidout(AActor* otherActor, UActorComponent* otherComponent);

	UFUNCTION(BlueprintCallable, Category = "Abilities | Voidout")
	void OnTowerEndOverlapVoidout(AActor* otherActor, UActorComponent* otherComponent);

		/* ... */

	UFUNCTION(BlueprintCallable, Category = "Menu | Pause Menu")
	void Menu();

	UFUNCTION(BlueprintCallable, Category = "Menu | Menu Base")
	void AddMenu(UUSWG_MenuBase* userWidget, int32 ZOrder = 0, bool showMenu = true, bool setActiveMenu = true, bool showMouseCursor = true, bool pauseGame = true);

	UFUNCTION(BlueprintCallable, Category = "Menu | Menu Base")
	void ShowMenu(UUSWG_MenuBase* userWidget, bool showMenu = true, bool resetActiveMenu = true, bool showMouseCursor = true, bool pauseGame = true);

	UFUNCTION(BlueprintCallable, Category = "Menu | Menu Base")
	void RemoveMenu(UUSWG_MenuBase* userWidget, bool resetActiveMenu = true, bool hideMouseCursor = true, bool resumeGame = true);

	bool ShowHUDBase(bool hide) const;

	UFUNCTION(BlueprintCallable, Category = "Menu | Tutorial Menu")
	inline UUSWG_MenuBase* GetTutorial() const noexcept { return tutorial; }

	UFUNCTION(BlueprintCallable, Category = "Menu | Tutorial Menu")
	bool ShowTutorial(bool show = true, bool pause = true, bool setActive = true);

	bool ShowMouseCursor(bool show = true);

	void CallTimer(float timeToMarkPlayer);

	void UpdateBlackBoardGlobal();
	
protected:

	/* FUNCTIONS */

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PlayerTick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

private:

	/* VARIABLES */

		/* Common */

	FVector2D lastMousePosition{ FVector2D::ZeroVector };

	EMouseCursor::Type gameCursor{ EMouseCursor::Crosshairs };

	EMouseCursor::Type menuCursor{ EMouseCursor::Default };

	float moveUpDownValue{ 0.0f };

	float moveRightLeftValue{ 0.0f };

	float sightUpDownValue{ 0.0f };

	float sightRightLeftValue{ 0.0f };

	bool isSightMoved{ false };

	float sightDeadZone{ 0.3f };

		/* Dark Slash */

	bool isKilling{ false };

	TArray<class ACHRT_Enemy*> reachableEnemiesDarkSlash{};

	TArray<class AACTR_DestroyableLight*> reachableDestroyableLights{};

		/* Shadow Dash */

	bool isShadowDashReady{ true };

	bool isShadowDashIndicatorShow{ false };

	bool isShadowDashing{ false };

	float shadowDashCooldownWaited{ 0.0f };

	const float SHADOW_DASH_FRICTION{ 9.8f };

	const float SHADOW_DASH_INDICATOR_X_SIZE{ 1920.f };

	const float SHADOW_DASH_INDICATOR_Y_SIZE{ 1080.f };

	TArray<ACHRT_Enemy*> reachableEnemiesShadowDash{};

		/* Voidout */

	bool isVoidoutReady{ true };

	float voidoutCooldownWaited{ 0.0f };

	TArray<class AACTR_SwitchableLight*> reachableTowers;

	AACTR_SwitchableLight* selectedTower{ nullptr };

		/* User Widgets */

	bool isMenuShow{ false };

	TSubclassOf<UUSWG_MenuBase> startMenuClass{ nullptr };

	TSubclassOf<UUSWG_MenuBase> tutorialClass{ nullptr };

	UUSWG_MenuBase* tutorial{ nullptr };

	TSubclassOf<class UUserWidget> shadowDashIndicatorClass{ nullptr };

	class AHUD_Base* baseHUD{ nullptr };

	FTimerHandle m_timerHandler;
	
	/* FUNCTIONS */

	void HoldKeysComprobations();

	void InitiateVariables();

	void InitiatePercents();

	class UGMIN_Base* GetGameInstance();

	bool IsUsingController();

	class ACHRT_Player* GetCHRTPlayer() const;

	class UACMP_Health* GetHealthComp() const;

	void AnyInput(FKey key);

	void AnyInputGamepad();

	void AnyInputPC();

	void MoveUpDown(float value);

	void MoveRightLeft(float value);

	void SightUpDown(float value);

	void SightRightLeft(float value);

	static bool IsValueOutsideRange(float value, float range);

	void Skip();

	void SkipPressed(FKey key);

	void SkipReleased(FKey key);

		/* Dark Slash */

	void CheckActionIndicatorShow();

	void SetActionIndicatorShow(bool show = true);

	void SetMarkedByFlareShow(bool show);

	void DarkSlash();

	static int KillReachableEnemy(TArray<ACHRT_Enemy*> reachableEnemies, FVector& location);

	static ACHRT_Enemy* GetReachableEnemy(TArray<ACHRT_Enemy*> reachableEnemies);

	bool DestroyReachableLight(FVector& location);

	class AACTR_DestroyableLight* GetReachableDestroyableLight();

	void ToggleKilling();

		/* Shadow Dash */

	void UpdateShadowDashCooldownWaited(const float deltaTime);

	void UpdatePercentShadowDash(float Value, float max_coolDown);

	void SetShadowDashReady(bool glow = false);

	void UpdateShadowDashIndicatorByMouse(FVector2D mousePositionCorrected);

	void UpdateShadowDashIndicatorByMove();

	void SetShadowDashIndicatorByCoordenate(const FVector2D& vector2D) const;

	void ShadowDashPressed();

	void DoShadowDash();

	FRotator GetShadowDashIndicatorRotation() const;

	void DoShadowDashLaunch() const;

	void StopShadowDash();

	FRotator GetRotationXYToLocation(const FVector location);

		/* Voidout */

	void Voidout();

	void EndVoidoutAnimation();

	void VoidoutExplosion();

	void UpdateSelectedTowerToVoidout();

	void UpdateVoidoutCooldownWaited(float DeltaTime);

	void UpdatePercentVoidout(float Value, float max_coolDown);

	void SetVoidoutReady(bool glow = false);

	AACTR_SwitchableLight* GetReachableTower();

		/* Menu Controls */

	void UpPressed(FKey key);

	void DownPressed(FKey key);

	void RightPressed(FKey key);

	void RightReleased(FKey key);

	void LeftPressed(FKey key);

	void LeftReleased(FKey key);

	void SelectPressed();

	void BackPressed();

	void NextPagePressed();

	void PreviousPagePressed();

		/* User Widgets */

	void SetWidgetShow(UUserWidget* userWidget, const bool show = true);
	
	void TogglePauseMenuShow();

	void SetMouseCursor(bool showMouseCursor, EMouseCursor::Type mouseCursor);

	void ToggleUnlockShadowDash();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	// Cheats

	void ToogleShowCheats();

	void ToogleInvincible();

	void LoadNextCheckpoint();

	void LoadPreviousCheckpoint();

	void ToggleUnlockDirectionalShadowDash();

	void ToggleUnlockVoidout();

	void ForceSkip();

#endif

};