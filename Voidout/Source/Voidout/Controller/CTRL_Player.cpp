
#include "CTRL_Player.h"
#include "AIController/BLKB_Global.h"
#include "Animations/ANIM_Player.h"
#include "Assets/ACTR_DestroyableLight.h"
#include "Assets/ACTR_SequenceTrigger.h"
#include "Character/CHRT_Enemy.h"
#include "Character/CHRT_Player.h"
#include "Components/ACMP_Health.h"
#include "Components/ACMP_LightStructureSubscription.h"
#include "Assets/ACTR_SwitchableLight.h"
#include "GameInstance/GMIN_Base.h"
#include "HUD/HUD_Base.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SoundEventManager.h"
#include "System/SYST_SystemsManager.h"
#include "UserWidget/USWG_MenuBase.h"

#include <math.h>
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/ProgressBar.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GenericPlatform/ICursor.h"
#include "InputCoreTypes.h"
#include "Math/Vector.h"
#include "Kismet/KismetInputLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequence.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"


ACTRL_Player::ACTRL_Player()
{
	static ConstructorHelpers::FClassFinder<UUSWG_MenuBase> USWG_StartMenu_Class(TEXT("/Game/Blueprints/UserWidget/Menu/USWG_StartMenu"));
	startMenuClass = USWG_StartMenu_Class.Class;

	static ConstructorHelpers::FClassFinder<UUSWG_MenuBase> USWG_Tutorial_Class(TEXT("/Game/Blueprints/UserWidget/Tutorials/USWG_Tutorial"));
	tutorialClass = USWG_Tutorial_Class.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> USWG_ShadowDashIndicator_Class(TEXT("/Game/Blueprints/UserWidget/Indicators/USWG_ShadowDashIndicator"));
	shadowDashIndicatorClass = USWG_ShadowDashIndicator_Class.Class;

	// Set teleport.
	m_teleporter = std::make_unique<SYST_Teleporter>();

	m_restarted = false;
	
	SetTickableWhenPaused(true);
}

void ACTRL_Player::BeginPlay()
{
	Super::BeginPlay();

	InitiateVariables();
}

void ACTRL_Player::InitiateVariables()
{
	// Mouse Cursor
	SetMouseCursor(true, gameCursor);

	shadowDashIndicator = CreateWidget<UUserWidget>(this, shadowDashIndicatorClass);
	shadowDashIndicator->AddToViewport(0);
	SetShadowDashIndicatorShow(false);

	tutorial = CreateWidget<UUSWG_MenuBase>(this, tutorialClass);
	AddMenu(tutorial, 997, false, false, false, false);
	
	startMenu = CreateWidget<UUSWG_MenuBase>(this, startMenuClass);
	AddMenu(startMenu, 999);

	// HUD
	baseHUD = Cast<AHUD_Base>(GetHUD());	
	ResetState();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// Cheat codes
	USYST_LogManager::PrintSpecial(this, "You can skip the cutscene pressing Y on Xbox Controller or Ctrl + Enter on Keyboard");
	USYST_LogManager::PrintSpecial(this, "To show cheat codes press X on Xbox Controller or Ctrl+C on Keyboard");
#endif
}

bool ACTRL_Player::ShowHUDBase(bool show) const
{
	if (baseHUD == nullptr)
		return false;
	
	return baseHUD->ShowAll(show);
}

bool ACTRL_Player::ShowTutorial(bool show, bool pause, bool setActive)
{
	if (tutorial == nullptr)
		return false;
	
	bool visibleBefore = tutorial->IsVisible();
	if (show)
	{
		if (GetGameInstance()->playerStatus == EPlayerStatus::InGame)
		{
			activeMenu = setActive ? tutorial : activeMenu;
			tutorial->Show();
			SetMouseCursor(!IsUsingController(), menuCursor);
			SetPause(pause);
		}
		else
		{
			return show;
		}
	}
	else
	{
		activeMenu = setActive ? nullptr : activeMenu;
		tutorial->Hide();
		SetMouseCursor(!IsUsingController(), gameCursor);
		SetPause(false);
	}
	return visibleBefore;
}

void ACTRL_Player::OnStartGame()
{
	InitiatePercents();
}

void ACTRL_Player::InitiatePercents()
{
	UpdatePercentShadowDash(isShadowDashUnlocked ? shadowDashCooldown : 0, shadowDashCooldown);
	UpdatePercentVoidout(isVoidoutUnlocked ? voidoutCooldown : 0, voidoutCooldown);
}

void ACTRL_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld() != nullptr && GetWorld()->GetGameViewport() != nullptr)
	{
		FVector2D mousePosition, viewportSize;
		GetWorld()->GetGameViewport()->GetMousePosition(mousePosition);
		GetWorld()->GetGameViewport()->GetViewportSize(viewportSize);

		if (mousePosition.X >= viewportSize.X || mousePosition.X <= 0
			|| mousePosition.Y >= viewportSize.Y || mousePosition.Y <= 0)
			return;

		FVector2D mousePositionCorrected = FVector2D(mousePosition.X - (viewportSize.X / 2), -(mousePosition.Y - (viewportSize.Y / 2))).GetRotated(90.f);

		if (mousePositionCorrected != lastMousePosition)
		{
			if (IsUsingController() && lastMousePosition != FVector2D::ZeroVector)
			{
				AnyInputPC();
			}
			lastMousePosition = mousePositionCorrected;
			UpdateShadowDashIndicatorByMouse(mousePositionCorrected);
		}
		else if (!IsUsingController())
		{
			UpdateShadowDashIndicatorByMouse(mousePositionCorrected);
		}
	}

	HoldKeysComprobations();
}

void ACTRL_Player::HoldKeysComprobations()
{
	// Skip hold comprobation
	if (skipKey != EKeys::Invalid && GetGameInstance()->sequenceTrigger != nullptr
		&& GetInputKeyTimeDown(skipKey) >= GetGameInstance()->sequenceTrigger->timeHoldingToSkipSequence)
		Skip();

	// Right hold comprobation
	if (rightKey != EKeys::Invalid && GetInputKeyTimeDown(rightKey) >= timeUntilKeyHeld)
	{
		int rightKeyHeldCounterAux = floor((GetInputKeyTimeDown(rightKey) - timeUntilKeyHeld) / timeBetweenKeyHeldTicks);
		if (rightKeyHeldCounterAux > rightKeyHeldCounter)
			for (int i = 0; i < rightKeyHeldCounterAux - rightKeyHeldCounter && i < 2; ++i)
				RightPressed(rightKey);

		rightKeyHeldCounter = rightKeyHeldCounterAux;
	}

	// Left hold comprobation
	if (leftKey != EKeys::Invalid && GetInputKeyTimeDown(leftKey) >= timeUntilKeyHeld)
	{
		int leftKeyHeldCounterAux = floor((GetInputKeyTimeDown(leftKey) - timeUntilKeyHeld) / timeBetweenKeyHeldTicks);
		if (leftKeyHeldCounterAux > leftKeyHeldCounter)
			for (int i = 0; i < leftKeyHeldCounterAux - leftKeyHeldCounter && i < 2; ++i)
				LeftPressed(leftKey);

		leftKeyHeldCounter = leftKeyHeldCounterAux;
	}
}

void ACTRL_Player::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(isShadowDashUnlocked)
	{
		UpdateShadowDashCooldownWaited(DeltaTime);
	}

	if (isVoidoutUnlocked)
	{
		UpdateSelectedTowerToVoidout();
		UpdateVoidoutCooldownWaited(DeltaTime);
	}
}

void ACTRL_Player::UpdateShadowDashCooldownWaited(const float deltaTime)
{
	// If we are shadow dashing
	if (isShadowDashReady)
		return;
	
	// We update it in every tick
	shadowDashCooldownWaited += deltaTime;
				
	// If he is shadow dashing and the duration has ended
	if (isShadowDashing && shadowDashCooldownWaited >= shadowDashDuration)
	{
		// We set the shadow dash as ended
		isShadowDashing = false;

		GetCHRTPlayer()->StopShadowDashPS();
	}

	// If the time waited is greater or equal to the cooldown
	if (shadowDashCooldownWaited >= shadowDashCooldown)
		SetShadowDashReady(true);
	else
		UpdatePercentShadowDash(shadowDashCooldownWaited, shadowDashCooldown);
}

void ACTRL_Player::UpdatePercentShadowDash(float Value, float max_coolDown)
{
	if (baseHUD == nullptr) return;
	
	if (isShadowDashUnlocked)
	{
		baseHUD->UpdatePercentShadowDash(Value, max_coolDown);
	}
	else
	{
		baseHUD->UpdatePercentShadowDash(0, max_coolDown);
	}
}

void ACTRL_Player::SetShadowDashReady(bool glow)
{
	isShadowDashReady = true;
	shadowDashCooldownWaited = 0.0f;

	if (glow && baseHUD != nullptr)
		baseHUD->DoShadowDashGlow();

	UpdatePercentShadowDash(shadowDashCooldown, shadowDashCooldown);
}

UGMIN_Base* ACTRL_Player::GetGameInstance()
{
	return Cast<UGMIN_Base>(UGameplayStatics::GetGameInstance(GetWorld()));
}

bool ACTRL_Player::IsUsingController()
{
	UGMIN_Base* gameInstanceBase = Cast<UGMIN_Base>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (gameInstanceBase != nullptr)
	{
		return gameInstanceBase->isUsingController;
	}
	else
	{
		USYST_LogManager::PrintError(this, "ACTRL_Player.IsUsingController: No baseGMIN");
	}
	return false;
}

ACHRT_Player* ACTRL_Player::GetCHRTPlayer() const
{
	return Cast<ACHRT_Player>(GetPawnOrSpectator());
}

UACMP_Health* ACTRL_Player::GetHealthComp() const
{
	return Cast<UACMP_Health>(GetComponentByClass(UACMP_Health::StaticClass()));
}

void ACTRL_Player::ResetState()
{
	//Input reset
	ResetIgnoreMoveInput();

	// Dark Slash reset
	if (isKilling)
		ToggleKilling();
	reachableEnemiesDarkSlash.Empty();
	reachableDestroyableLights.Empty();

	// Shadow Dash reset
	SetShadowDashIndicatorShow(false);
	isShadowDashing = false;
	SetShadowDashReady();
	reachableEnemiesShadowDash.Empty();

	// Voidout reset
	SetVoidoutReady();
	reachableTowers.Empty();

	SetShadowDashIndicatorByCoordenate(FVector2D(-1, 1));
	
	if (BLKB_Global::getSeenByFlare() && !isShadowDashUnlocked)
	{
		ToggleUnlockShadowDash();
	}
}

void ACTRL_Player::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Any Inputs
	InputComponent->BindAction("AnyInput", IE_Pressed, this, &ACTRL_Player::AnyInput).bExecuteWhenPaused = true;

	// Movement
	InputComponent->BindAxis("MoveUpDown", this, &ACTRL_Player::MoveUpDown);
	InputComponent->BindAxis("MoveRightLeft", this, &ACTRL_Player::MoveRightLeft);

	// Sight
	InputComponent->BindAxis("SightUpDown", this, &ACTRL_Player::SightUpDown);
	InputComponent->BindAxis("SightRightLeft", this, &ACTRL_Player::SightRightLeft);

	// Attack
	InputComponent->BindAction("DarkSlash", IE_Pressed, this, &ACTRL_Player::DarkSlash);

	// Abilities
	InputComponent->BindAction("ShadowDash", IE_Pressed, this, &ACTRL_Player::ShadowDashPressed);
	InputComponent->BindAction("Voidout", IE_Pressed, this, &ACTRL_Player::Voidout);

	// Menu navigation
	InputComponent->BindAction("Menu", IE_Pressed, this, &ACTRL_Player::Menu).bExecuteWhenPaused = true;
	InputComponent->BindAction("Up", IE_Pressed, this, &ACTRL_Player::UpPressed).bExecuteWhenPaused = true;
	InputComponent->BindAction("Down", IE_Pressed, this, &ACTRL_Player::DownPressed).bExecuteWhenPaused = true;
	InputComponent->BindAction("Right", IE_Pressed, this, &ACTRL_Player::RightPressed).bExecuteWhenPaused = true;
	InputComponent->BindAction("Right", IE_Released, this, &ACTRL_Player::RightReleased).bExecuteWhenPaused = true;
	InputComponent->BindAction("Left", IE_Pressed, this, &ACTRL_Player::LeftPressed).bExecuteWhenPaused = true;
	InputComponent->BindAction("Left", IE_Released, this, &ACTRL_Player::LeftReleased).bExecuteWhenPaused = true;
	InputComponent->BindAction("Select", IE_Pressed, this, &ACTRL_Player::SelectPressed).bExecuteWhenPaused = true; 
	InputComponent->BindAction("Back", IE_Pressed, this, &ACTRL_Player::BackPressed).bExecuteWhenPaused = true;
	InputComponent->BindAction("NextPage", IE_Pressed, this, &ACTRL_Player::NextPagePressed).bExecuteWhenPaused = true;
	InputComponent->BindAction("PreviousPage", IE_Pressed, this, &ACTRL_Player::PreviousPagePressed).bExecuteWhenPaused = true;

	// Level Sequences
	InputComponent->BindAction("Skip", IE_Pressed, this, &ACTRL_Player::SkipPressed).bExecuteWhenPaused = true;
	InputComponent->BindAction("Skip", IE_Released, this, &ACTRL_Player::SkipReleased).bExecuteWhenPaused = true;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	// Cheats
	InputComponent->BindAction("CheatInvicible", IE_Pressed, this, &ACTRL_Player::ToogleInvincible);
	InputComponent->BindAction("CheatLoadNextCheckpoint", IE_Pressed, this, &ACTRL_Player::LoadNextCheckpoint);
	InputComponent->BindAction("CheatUnlockShadowDash", IE_Pressed, this, &ACTRL_Player::ToggleUnlockShadowDash);
	InputComponent->BindAction("CheatUnlockDirectionalShadowDash", IE_Pressed, this, &ACTRL_Player::ToggleUnlockDirectionalShadowDash);
	InputComponent->BindAction("CheatUnlockVoidout", IE_Pressed, this, &ACTRL_Player::ToggleUnlockVoidout);
	InputComponent->BindAction("CheatForceSkip", IE_Pressed, this, &ACTRL_Player::ForceSkip);
	InputComponent->BindAction("CheatShowCheats", IE_Pressed, this, &ACTRL_Player::ToogleShowCheats);

#endif
}

void ACTRL_Player::AnyInput(FKey key)
{
	if (UKismetInputLibrary::Key_IsGamepadKey(key))
	{
		AnyInputGamepad();
	}
	else if (UKismetInputLibrary::Key_IsKeyboardKey(key) || UKismetInputLibrary::Key_IsMouseButton(key))
	{
		AnyInputPC();
	}
}

void ACTRL_Player::AnyInputGamepad()
{
	if (!IsUsingController())
	{
		GetGameInstance()->isUsingController = true;
		SetMouseCursor(false, activeMenu != nullptr ? menuCursor : gameCursor);
	}
}

void ACTRL_Player::AnyInputPC()
{
	if (IsUsingController())
	{
		GetGameInstance()->isUsingController = false;
		SetMouseCursor(true, activeMenu != nullptr ? menuCursor : gameCursor);
	}
}

void ACTRL_Player::MoveUpDown(float Value)
{
	moveUpDownValue = Value;

	if (Value == 0.0f) return;

	// find out which way is forward
	const FRotator YawRotation(0, GetControlRotation().Yaw, 0);

	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// add movement in that direction
	GetCHRTPlayer()->AddMovementInput(Direction, Value);

	UpdateShadowDashIndicatorByMove();
}

void ACTRL_Player::MoveRightLeft(float Value)
{
	moveRightLeftValue = Value;

	if (Value == 0.0f) return;

	// find out which way is forward
	const FRotator YawRotation(0, GetControlRotation().Yaw, 0);

	// get right vector 
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	// add movement in that direction
	GetCHRTPlayer()->AddMovementInput(Direction, Value);

	UpdateShadowDashIndicatorByMove();
}

void ACTRL_Player::UpdateShadowDashIndicatorByMouse(FVector2D mousePositionCorrected)
{
	if (!isDirectionalShadowDashUnlocked) return;
	isSightMoved = true;
	SetShadowDashIndicatorShow(!isKilling && isShadowDashReady && isShadowDashUnlocked);
	SetShadowDashIndicatorByCoordenate(mousePositionCorrected);
}

void ACTRL_Player::UpdateShadowDashIndicatorByMove()
{
	if (isSightMoved) return;
	const FVector2D vector2D = FVector2D(moveUpDownValue, -moveRightLeftValue).GetRotated(180.0f);
	SetShadowDashIndicatorByCoordenate(vector2D);
}

void ACTRL_Player::SightUpDown(float Value)
{
	if(!IsUsingController() || !isShadowDashUnlocked || !isDirectionalShadowDashUnlocked || !isShadowDashReady)
		return;
	
	sightUpDownValue = Value;

	if (IsValueOutsideRange(Value, sightDeadZone))
	{
		isSightMoved = true;
		SetShadowDashIndicatorByCoordenate(FVector2D(sightUpDownValue, sightRightLeftValue));

		if (!isShadowDashIndicatorShow)
		{
			SetShadowDashIndicatorShow(isSightMoved && !isKilling && isShadowDashReady && isShadowDashUnlocked);
		}
	}
	else if (!IsValueOutsideRange(sightRightLeftValue, sightDeadZone))
	{
		isSightMoved = false;

		if (isShadowDashIndicatorShow)
		{
			SetShadowDashIndicatorShow(isSightMoved && !isKilling && isShadowDashReady && isShadowDashUnlocked);
		}
	}
}

void ACTRL_Player::SightRightLeft(float Value)
{
	if (!IsUsingController() || !isShadowDashUnlocked || !isDirectionalShadowDashUnlocked || !isShadowDashReady)
		return;
	
	sightRightLeftValue = Value;

	if (IsValueOutsideRange(Value, sightDeadZone))
	{
		isSightMoved = true;
		SetShadowDashIndicatorByCoordenate(FVector2D(sightUpDownValue, sightRightLeftValue));

		if (!isShadowDashIndicatorShow)
		{
			SetShadowDashIndicatorShow(isSightMoved && !isKilling && isShadowDashReady && isShadowDashUnlocked);
		}
	}
	else if (!IsValueOutsideRange(sightUpDownValue, sightDeadZone))
	{
		isSightMoved = false;

		if (isShadowDashIndicatorShow)
		{
			SetShadowDashIndicatorShow(isSightMoved && !isKilling && isShadowDashReady && isShadowDashUnlocked);
		}
	}
}

bool ACTRL_Player::IsValueOutsideRange(float value, float range)
{
	return (value > range || value < -range);
}

void ACTRL_Player::SetShadowDashIndicatorByCoordenate(const FVector2D& vector2D) const
{
	if (vector2D.IsZero())
		return;
	// We correct the coordenates introduced and normalize them
	FVector2D vector2DCorrected = vector2D.GetRotated(135.f);
	vector2DCorrected.Normalize();

	vector2DCorrected *= FVector2D(1, -1) * shadowDashDistance / SHADOW_DASH_FRICTION;

	auto* player = GetCHRTPlayer();
	if (!player) return;
	FVector worldPosition = player->GetActorLocation() + FVector(vector2DCorrected, 0.f);
	FVector2D screenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(this, worldPosition, screenPosition))
	{
		int32 sizeX, sizeY;
		GetViewportSize(sizeX, sizeY);

		// (- size / 2) sets it in the center of the screen
		// (SIZE_INDICATOR * XY / sizeViewport) converts the coordenates relatives to the viewport size, to relatives indicator size
		FVector2D screenPositionCorrected = FVector2D((SHADOW_DASH_INDICATOR_X_SIZE * (screenPosition.X - sizeX / 2) / sizeX), (SHADOW_DASH_INDICATOR_Y_SIZE * (screenPosition.Y - sizeY / 2) / sizeY));

		if (shadowDashIndicator != nullptr)
		{
			shadowDashIndicator->SetRenderTranslation(screenPositionCorrected);
		}
	}
}

void ACTRL_Player::UnlockShadowDash()
{
	isShadowDashUnlocked = true;
	UpdatePercentShadowDash(shadowDashCooldown, shadowDashCooldown);
}

void ACTRL_Player::OnSphereBeginOverlapDarkSlash(AActor* otherActor, UActorComponent* otherComp)
{
	if (Cast<UMeshComponent>(otherComp) == nullptr && Cast<UCapsuleComponent>(otherComp) == nullptr)
		return;

	ACHRT_Enemy* enemy = Cast<ACHRT_Enemy>(otherActor);
	if (enemy != nullptr)
	{
		if (enemy->m_health != nullptr && !enemy->IsDying())
		{
			reachableEnemiesDarkSlash.Add(enemy);
			CheckActionIndicatorShow();
		}
	}
	else
	{
		AACTR_DestroyableLight* energyTorche = Cast<AACTR_DestroyableLight>(otherActor);
		if (energyTorche != nullptr)
		{
			reachableDestroyableLights.Add(energyTorche);
			CheckActionIndicatorShow();
		}
	}
}

void ACTRL_Player::OnSphereEndOverlapDarkSlash(AActor* otherActor, UActorComponent* otherComp)
{
	if (Cast<UMeshComponent>(otherComp) == nullptr && Cast<UCapsuleComponent>(otherComp) == nullptr)
		return;

	ACHRT_Enemy* enemy = Cast<ACHRT_Enemy>(otherActor);
	if (enemy != nullptr)
	{
		reachableEnemiesDarkSlash.Remove(enemy);
		CheckActionIndicatorShow();
	}
	else
	{
		AACTR_DestroyableLight* energyTorche = Cast<AACTR_DestroyableLight>(otherActor);
		if (energyTorche != nullptr)
		{
			reachableDestroyableLights.Remove(energyTorche);
			CheckActionIndicatorShow();
		}
	}
}

void ACTRL_Player::CheckActionIndicatorShow()
{
	if (reachableEnemiesDarkSlash.Num() == 1
		|| reachableDestroyableLights.Num() == 1) {

		SetActionIndicatorShow();
	}
	else if (reachableEnemiesDarkSlash.Num() == 0
		&& reachableDestroyableLights.Num() == 0)
	{
		SetActionIndicatorShow(false);
	}
}

void ACTRL_Player::SetActionIndicatorShow(bool show)
{
	if (darkSlashIndicator != nullptr)
	{
		SetWidgetShow(darkSlashIndicator->GetUserWidgetObject(), show);
	}
}

void ACTRL_Player::SetMarkedByFlareShow(bool show)
{
	if (markedByFlareIndicator != nullptr)
	{
		SetWidgetShow(markedByFlareIndicator->GetUserWidgetObject(), show);
	}
	else
		USYST_LogManager::PrintSpecial(this, "flare mark null");
}

void ACTRL_Player::DarkSlash()
{
	if (!isKilling && (reachableEnemiesDarkSlash.Num() > 0 || reachableDestroyableLights.Num() > 0)
		&& GetGameInstance()->playerStatus != EPlayerStatus::InSequence)
	{
		ToggleKilling();

		FTimerHandle timerHandler;
		GetWorld()->GetTimerManager().SetTimer(timerHandler, this, &ACTRL_Player::DarkSlashEnded, 2.f, false);
	}
}

bool ACTRL_Player::DarkSlashDamage()
{
	// Play the Dark Slash sound
	SYST_SystemsManager::GetSoundEventManager()->playPlayerDarkSlash(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());

	if (darkSlashDisablesControls || IgnoreMoveInput)
		ResetIgnoreMoveInput();

	FVector location = FVector::ZeroVector;
	int resultEnemy = KillReachableEnemy(reachableEnemiesDarkSlash, location);
	bool resultTorche = resultEnemy ? false : DestroyReachableLight(location);

	if (resultEnemy || resultTorche)
	{
		// Rotate towards the killed/destroyed element
		GetCHRTPlayer()->GetRootComponent()->SetWorldRotation(GetRotationXYToLocation(location));

		// Only if it's a kill
		if (resultEnemy == 2 || resultTorche)
		{
			// Shadow dash ready on kill
			SetShadowDashReady();

			// Reduce the shadow voidout time on kill
			UpdateVoidoutCooldownWaited(voidoutCooldownReductionOnKill);

			// If desired, the kill gives health back
			if (darkSlashKillGivesHealth)
				GetCHRTPlayer()->LifeRecover(darkSlashKillHealth);
		}

		return true;
	}
	return false;
}

void ACTRL_Player::DarkSlashEnded()
{
	if (isKilling)
		ToggleKilling();
}

// Take damage. 0 -> Nothing, 1 -> Damaged, 2 -> Dead
int ACTRL_Player::KillReachableEnemy(TArray<ACHRT_Enemy*> reachableEnemies, FVector& location)
{
	ACHRT_Enemy* enemy = GetReachableEnemy(reachableEnemies);
	if (enemy != nullptr)
	{
		location = enemy->GetActorLocation();
		return enemy->TakeDamage(100);
	}

	location = FVector::ZeroVector;
	return 0;
}

ACHRT_Enemy* ACTRL_Player::GetReachableEnemy(TArray<ACHRT_Enemy*> reachableEnemies)
{
	if (reachableEnemies.Num() <= 0)
		return nullptr;

	return reachableEnemies.Top();
}

bool ACTRL_Player::DestroyReachableLight(FVector& location)
{
	AACTR_DestroyableLight* reachableEnergyTorche = GetReachableDestroyableLight();
	if (reachableEnergyTorche != nullptr)
	{
		location = reachableEnergyTorche->GetActorLocation();
		reachableEnergyTorche->Death();
		reachableEnergyTorche->Destroy();
		return true;
	}

	location = FVector::ZeroVector;
	return false;
}

AACTR_DestroyableLight* ACTRL_Player::GetReachableDestroyableLight()
{
	if (reachableDestroyableLights.Num() <= 0)
		return nullptr;

	return reachableDestroyableLights.Top();
}

void ACTRL_Player::ToggleKilling()
{
	if (GetCHRTPlayer() == nullptr || GetCHRTPlayer()->GetMesh() == nullptr)
		return;

	isKilling = !isKilling;
	if (isKilling)
	{
		UANIM_Player* playerAnimations = Cast<UANIM_Player>(GetCHRTPlayer()->GetMesh()->AnimScriptInstance);
		if(playerAnimations != nullptr)
			playerAnimations->SetIsSlashing(true);
		if(darkSlashDisablesControls)
			SetIgnoreMoveInput(true);
	}
	else
	{
		UANIM_Player* playerAnimations = Cast<UANIM_Player>(GetCHRTPlayer()->GetMesh()->AnimScriptInstance);
		if (playerAnimations != nullptr)
			playerAnimations->SetIsSlashing(false);
		if (darkSlashDisablesControls || IgnoreMoveInput)
			ResetIgnoreMoveInput();
	}
}

void ACTRL_Player::OnSphereBeginOverlapShadowDash(AActor* otherActor, UActorComponent* otherComp)
{
	UMeshComponent* meshComponent = Cast<UMeshComponent>(otherComp);
	if (meshComponent == nullptr) return;
	ACHRT_Enemy* enemy = Cast<ACHRT_Enemy>(otherActor);
	if (enemy == nullptr) return;
	
	reachableEnemiesShadowDash.Add(enemy);

	if (isShadowDashing)
	{
		StopShadowDash();
	}
}

void ACTRL_Player::OnSphereEndOverlapShadowDash(AActor* otherActor, UActorComponent* otherComp)
{
	UMeshComponent* meshComponent = Cast<UMeshComponent>(otherComp);
	if (meshComponent == nullptr) return;
	ACHRT_Enemy* enemy = Cast<ACHRT_Enemy>(otherActor);
	if (enemy == nullptr) return;
	
	reachableEnemiesShadowDash.Remove(enemy);
}

void ACTRL_Player::ShadowDashPressed()
{
	if(GetGameInstance()->playerStatus != EPlayerStatus::InSequence)
	{
		if (BLKB_Global::getSeenByFlare() || !isShadowDashReady)
		{
			SYST_SystemsManager::GetSoundEventManager()->playUnreadySkill();
			return;
		}
		SetShadowDashIndicatorShow(!isKilling && isShadowDashReady && isShadowDashUnlocked);
		DoShadowDash();
	}
}

bool ACTRL_Player::SetShadowDashIndicatorShow(bool show)
{
	bool shadowDashIndicatorShowBefore = isShadowDashIndicatorShow;
	isShadowDashIndicatorShow = show;
	SetWidgetShow(shadowDashIndicator, isShadowDashIndicatorShow);
	return shadowDashIndicatorShowBefore;
}

void ACTRL_Player::DoShadowDash()
{
	// If the sight is moved, is not killing, the shadow dash is ready and unlocked, we show the indicator
	if (isKilling || !isShadowDashReady || !isShadowDashIndicatorShow || !isShadowDashUnlocked)
		return;

	// We hide the indicator, indicate that we are dashing, and is not ready
	SetShadowDashIndicatorShow(false);
	isShadowDashing = true;
	isShadowDashReady = false;

	// We rotate the player towards the dash direction
	GetCHRTPlayer()->GetRootComponent()->SetWorldRotation(GetShadowDashIndicatorRotation());

	if(reachableEnemiesShadowDash.Num() < 1)
		// We do the dash
		DoShadowDashLaunch();
	else
	{
		FTimerHandle timerHandler;
		GetWorld()->GetTimerManager().SetTimer(timerHandler, this, &ACTRL_Player::StopShadowDash, 0.1f, false);
	}

	// Ignore the movement input
	SetIgnoreMoveInput(true);

	// Play the Shadow Dash Particle System
	GetCHRTPlayer()->StartShadowDashPS();

	FTimerHandle timerHandler1;
	GetWorld()->GetTimerManager().SetTimer(timerHandler1, GetCHRTPlayer(), &ACHRT_Player::StopShadowDashPS, shadowDashDuration, false);

	FTimerHandle timerHandler2;
	GetWorld()->GetTimerManager().SetTimer(timerHandler2, this, &ACTRL_Player::ResetIgnoreMoveInput, shadowDashDuration, false);

	// Play the Shadow Dash sound
	SYST_SystemsManager::GetSoundEventManager()->playPlayerShadowDash(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());
}

FRotator ACTRL_Player::GetShadowDashIndicatorRotation() const
{
	if (shadowDashIndicator == nullptr)
		return{ FRotator::ZeroRotator };

	const FVector2D translation = shadowDashIndicator->RenderTransform.Translation;
	const FRotator rotator = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, FVector(translation, 0.0f));
	return FRotator(0.0f, rotator.Yaw + 135.0f, 0.0f);
}

void ACTRL_Player::DoShadowDashLaunch() const
{
	if (shadowDashIndicator == nullptr)
		return;

	// Start location of the dash
	FVector startLocation = GetCHRTPlayer()->GetActorLocation();

	// 2D location of the ability corrected
	FVector2D abilityTranslation = shadowDashIndicator->RenderTransform.Translation;
	abilityTranslation.Normalize();
	abilityTranslation *= shadowDashDistance;
	abilityTranslation = UKismetMathLibrary::GetRotated2D(abilityTranslation, 135);

	// We correct the velocity to avoid inercy
	const FVector velocityCorrected = FVector(abilityTranslation, 0.0f) - GetCHRTPlayer()->GetVelocity();

	// Launch the character
	GetCHRTPlayer()->LaunchCharacter(velocityCorrected, false, false);
}

void ACTRL_Player::StopShadowDash()
{
	// Dashing stoped
	isShadowDashing = false;

	// Stop the character
	GetCHRTPlayer()->GetMovementComponent()->Velocity = FVector::ZeroVector;

	// Stop the Shadow Dash Particle System
	GetCHRTPlayer()->StopShadowDashPS();

	// Reset the ignoration of movement input
	ResetIgnoreMoveInput();

	// If there are any, kill an enemy
	FVector location;
	int result = KillReachableEnemy(reachableEnemiesShadowDash, location);
	if (!result) return;
	// Rotate the player towards the enemy killed
	GetCHRTPlayer()->GetRootComponent()->SetWorldRotation(GetRotationXYToLocation(location));

	// Only if it's a kill
	if (result == 2)
	{
		// Shadow Dash Cooldown reseted if you kill with it
		SetShadowDashReady();

		// If desired, the kill gives health back
		if (GetCHRTPlayer() != nullptr && shadowDashKillGivesHealth)
			GetCHRTPlayer()->LifeRecover(shadowDashKillHealth);

		ForceFeedbackShadowDashKill();
	}
}

FRotator ACTRL_Player::GetRotationXYToLocation(const FVector location)
{
	if(GetCHRTPlayer() != nullptr)
	{
		// We calculate the rotation of the player to look to the location, only in the XY axis
		FVector actorLocation = GetCHRTPlayer()->GetActorLocation();
		return UKismetMathLibrary::FindLookAtRotation(FVector(actorLocation.X, actorLocation.Y, 0), FVector(location.X, location.Y, 0));
	}
	return FRotator::ZeroRotator;
}

void ACTRL_Player::UnlockVoidout()
{
	isVoidoutUnlocked = true;
	UpdatePercentVoidout(voidoutCooldown, voidoutCooldown);
}

void ACTRL_Player::OnTowerBeginOverlapVoidout(AActor* otherActor, UActorComponent* otherComponent)
{
	AACTR_SwitchableLight* switchableLight = Cast<AACTR_SwitchableLight>(otherActor);
	if (switchableLight != nullptr && isVoidoutUnlocked && !Cast<USphereComponent>(otherComponent))
	{
		reachableTowers.Add(switchableLight);
		UpdateSelectedTowerToVoidout();
	}
}

void ACTRL_Player::OnTowerEndOverlapVoidout(AActor* otherActor, UActorComponent* otherComponent)
{
	AACTR_SwitchableLight* switchableLight = Cast<AACTR_SwitchableLight>(otherActor);
	if (switchableLight != nullptr && isVoidoutUnlocked && !Cast<USphereComponent>(otherComponent))
	{
		reachableTowers.Remove(switchableLight);
		switchableLight->SetActionIndicatorShow(false);
	}
}

void ACTRL_Player::Voidout()
{
	if (isKilling || !isVoidoutReady || !isVoidoutUnlocked || GetGameInstance()->playerStatus == EPlayerStatus::InSequence)
		return;

	selectedTower = GetReachableTower();
	if (selectedTower == nullptr || selectedTower->GetConnectionCount() > 0)
		return;

	isVoidoutReady = false;
	selectedTower->SetActionIndicatorShow(false);

	// Play the Voidout Particle System
	GetCHRTPlayer()->PlayVoidoutPS(selectedTower->GetActorLocation() + FVector{0.f, 0.f, 200.f});

	// Play the Voidout Animation
	if (GetCHRTPlayer() != nullptr || GetCHRTPlayer()->GetMesh() != nullptr)
	{
		UANIM_Player* playerAnimations = Cast<UANIM_Player>(GetCHRTPlayer()->GetMesh()->AnimScriptInstance);
		if (playerAnimations != nullptr)
			playerAnimations->SetIsVoidout(true);

		FTimerHandle timerHandler;
		GetWorld()->GetTimerManager().SetTimer(timerHandler, this, &ACTRL_Player::EndVoidoutAnimation, 3.f, false);
	}

	FTimerHandle timerHandler1;
	GetWorld()->GetTimerManager().SetTimer(timerHandler1, this, &ACTRL_Player::VoidoutExplosion, 0.8f, false);

	FTimerHandle timerHandler2;
	GetWorld()->GetTimerManager().SetTimer(timerHandler2, GetCHRTPlayer(), &ACHRT_Player::StopVoidoutPS, voidoutEffectDuration, false);
}

void ACTRL_Player::EndVoidoutAnimation()
{
	// Play the Voidout Particle System
	GetCHRTPlayer()->StopVoidoutPS();

	// Play the Voidout Animation
	if (GetCHRTPlayer() != nullptr || GetCHRTPlayer()->GetMesh() != nullptr)
	{
		UANIM_Player* playerAnimations = Cast<UANIM_Player>(GetCHRTPlayer()->GetMesh()->AnimScriptInstance);
		if (playerAnimations != nullptr)
			playerAnimations->SetIsVoidout(false);
	}
}

void ACTRL_Player::VoidoutExplosion()
{
	if (selectedTower == nullptr)
		return;

	selectedTower->SwitchLight(voidoutEffectDuration, voidoutEnemyImpulse);

	ForceFeedbackVoidoutExplosion();

	// Play the Voidout sound
	SYST_SystemsManager::GetSoundEventManager()->playPlayerVoidout(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());
}

void ACTRL_Player::UpdateSelectedTowerToVoidout()
{
	if (isVoidoutReady && reachableTowers.Num() > 0)
		GetReachableTower();
}

void ACTRL_Player::UpdateVoidoutCooldownWaited(float DeltaTime)
{
	if (isVoidoutReady) return;
	voidoutCooldownWaited += DeltaTime;

	if (voidoutCooldownWaited >= voidoutCooldown)
		SetVoidoutReady(true);
	else
		UpdatePercentVoidout(voidoutCooldownWaited, voidoutCooldown);
}

void ACTRL_Player::UpdatePercentVoidout(float Value, float max_coolDown)
{
	if (baseHUD == nullptr) return;
	
	if (isVoidoutUnlocked)
	{
		baseHUD->UpdatePercentVoidout(Value, max_coolDown);
	}
	else
	{
		baseHUD->UpdatePercentVoidout(0, max_coolDown);
	}
}

void ACTRL_Player::SetVoidoutReady(bool glow)
{
	isVoidoutReady = true;
	voidoutCooldownWaited = 0.0f;

	if (glow && baseHUD != nullptr)
	{
		baseHUD->DoVoidoutGlow();
		ForceFeedbackVoidoutReady();
	}

	UpdatePercentVoidout(voidoutCooldown, voidoutCooldown);
}

AACTR_SwitchableLight* ACTRL_Player::GetReachableTower()
{
	//Si hay al menos una torre en el array
	if (reachableTowers.Num() <= 0) return nullptr;
	float towerDistance = FVector::DistSquared(GetCHRTPlayer()->GetActorLocation(), reachableTowers[0]->GetActorLocation());
	AACTR_SwitchableLight* curTower = reachableTowers[0];

	for (auto* tower : reachableTowers)
	{
		if (tower == nullptr)
			continue;

		if (FVector::DistSquared(GetCHRTPlayer()->GetActorLocation(), tower->GetActorLocation()) < towerDistance)
		{
			curTower = tower;
			towerDistance = FVector::DistSquared(GetCHRTPlayer()->GetActorLocation(), tower->GetActorLocation());
		}
		else
			tower->SetActionIndicatorShow(false);
	}
	if (isVoidoutReady && curTower->isLightOnPermanent() && curTower->isLightOnVoidout() && curTower->GetConnectionCount() <= 0)
	{
		curTower->SetActionIndicatorShow(true);
	}
	else
	{
		curTower->SetActionIndicatorShow(false);
		return nullptr;//if the light is off voidout has not any effect.
	}
	return curTower;
}

void ACTRL_Player::UpPressed(FKey key)
{
	if (activeMenu != nullptr)
		activeMenu->AddButtonPosition(-1);
}

void ACTRL_Player::DownPressed(FKey key)
{
	if (activeMenu != nullptr)
		activeMenu->AddButtonPosition(1);
}

void ACTRL_Player::RightPressed(FKey key)
{
	if (leftKey == EKeys::Invalid && activeMenu != nullptr)
	{
		rightKey = key;
		activeMenu->Right();
	}
}

void ACTRL_Player::RightReleased(FKey key)
{
	if (rightKey == key)
	{
		rightKey = EKeys::Invalid;
		rightKeyHeldCounter = -1;
	}
}

void ACTRL_Player::LeftPressed(FKey key)
{
	if (rightKey == EKeys::Invalid && activeMenu != nullptr)
	{
		leftKey = key;
		activeMenu->Left();
	}
}

void ACTRL_Player::LeftReleased(FKey key)
{
	if (leftKey == key)
	{
		leftKey = EKeys::Invalid;
		leftKeyHeldCounter = -1;
	}
}

void ACTRL_Player::SelectPressed()
{
	if (activeMenu != nullptr)
		activeMenu->PressButton();
}

void ACTRL_Player::BackPressed()
{
	if (activeMenu != nullptr)
		activeMenu->Back();
}

void ACTRL_Player::NextPagePressed()
{
	if (activeMenu != nullptr)
		activeMenu->NextPage();
}

void ACTRL_Player::PreviousPagePressed()
{
	if (activeMenu != nullptr)
		activeMenu->PreviousPage();
}

void ACTRL_Player::SetWidgetShow(UUserWidget* userWidget, bool show)
{
	if (userWidget == nullptr)
		return;

	if (GetGameInstance()->playerStatus != EPlayerStatus::InSequence)
	{
		if (show)
		{
			userWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			userWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ACTRL_Player::Menu()
{
	if (pauseMenu != nullptr && (activeMenu == nullptr || activeMenu == pauseMenu)
		&& GetGameInstance()->playerStatus != EPlayerStatus::InSequence
		&& pauseMenu->pageSelected == "main")
	{
		TogglePauseMenuShow();
	}
}

void ACTRL_Player::TogglePauseMenuShow()
{
	isMenuShow = !isMenuShow;
	SetWidgetShow(pauseMenu, isMenuShow);
	activeMenu = isMenuShow ? pauseMenu : nullptr;
	if (activeMenu != nullptr)
	{
		GetGameInstance()->playerStatus = EPlayerStatus::InMenu;
		activeMenu->UpdateButtonPosition(0);
	}
	else
	{
		GetGameInstance()->playerStatus = EPlayerStatus::InGame;
	}
	if (isMenuShow) 
	{
		SYST_SystemsManager::GetSoundEventManager()->pauseMusicBus();
	}
	else 
	{
		if(!m_restarted)
		{
			SYST_SystemsManager::GetSoundEventManager()->playMusicBus();
		}
	}
	SetMouseCursor(!IsUsingController(), isMenuShow ? menuCursor : gameCursor);
	SetPause(isMenuShow);
}

void ACTRL_Player::AddMenu(UUSWG_MenuBase* userWidget, int32 ZOrder, bool showMenu, bool setActiveMenu, bool showMouseCursor, bool pauseGame)
{
	if (userWidget != nullptr)
	{
		userWidget->AddToViewport(ZOrder);
		ShowMenu(userWidget, showMenu, setActiveMenu, showMouseCursor, pauseGame);
	}
}

void ACTRL_Player::ShowMenu(UUSWG_MenuBase* userWidget, bool showMenu, bool setActiveMenu, bool showMouseCursor, bool pauseGame)
{
	if (userWidget == nullptr) return;
	SetWidgetShow(userWidget, showMenu);
	activeMenu = setActiveMenu ? userWidget : activeMenu;
	SetMouseCursor(!IsUsingController(), showMouseCursor ? menuCursor : gameCursor);
	SetPause(pauseGame);
}

void ACTRL_Player::RemoveMenu(UUSWG_MenuBase* userWidget, bool resetActiveMenu, bool hideMouseCursor, bool resumeGame)
{
	if (userWidget == nullptr) return;
	userWidget->RemoveFromParent();
	activeMenu = resetActiveMenu ? nullptr : activeMenu;
	SetMouseCursor(!IsUsingController(), hideMouseCursor ? gameCursor : menuCursor);
	SetPause(!resumeGame);
}

void ACTRL_Player::SetMouseCursor(bool showMouseCursor, EMouseCursor::Type mouseCursor)
{
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, activeMenu, EMouseLockMode::LockInFullscreen, false);
	ShowMouseCursor(showMouseCursor);
	CurrentMouseCursor = mouseCursor;
}

bool ACTRL_Player::ShowMouseCursor(bool show) 
{
	bool wasShowingMouseCursor = bShowMouseCursor;
	bShowMouseCursor = show;
	return wasShowingMouseCursor;
}

void ACTRL_Player::Skip()
{
	if (GetGameInstance()->sequenceTrigger != nullptr && GetGameInstance()->sequenceTrigger->canSkip)
		GetGameInstance()->sequenceTrigger->SkipTheLevelSequence();
}

void ACTRL_Player::SkipPressed(FKey key)
{
	if(GetGameInstance()->sequenceTrigger != nullptr && GetGameInstance()->sequenceTrigger->canSkip)
		skipKey = key;
}

void ACTRL_Player::SkipReleased(FKey key)
{
	if (skipKey == key)
		skipKey = EKeys::Invalid;
}

void ACTRL_Player::ToggleUnlockShadowDash()
{
	USYST_LogManager::PrintSpecial(this, "CHEAT: UnlockShadowDash called");

	isShadowDashUnlocked = !isShadowDashUnlocked;
	UpdatePercentShadowDash(shadowDashCooldown, shadowDashCooldown);
}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

// Cheats

void ACTRL_Player::ToogleShowCheats()
{
	USYST_LogManager::PrintSpecial(this, "CHEAT: ShowCheats called");

	if(baseHUD != nullptr && GetGameInstance()->playerStatus != EPlayerStatus::InSequence)
	{
		baseHUD->ToggleShowCheatsHelp();
	}
}

void ACTRL_Player::ToogleInvincible()
{
	USYST_LogManager::PrintSpecial(this, "CHEAT: Invincible called");

	ACHRT_Player* playerCharacter = GetCHRTPlayer();
	if (playerCharacter == nullptr) return;
	UACMP_Health* playerHealth = Cast<UACMP_Health>(playerCharacter->GetComponentByClass(UACMP_Health::StaticClass()));
	if (playerHealth == nullptr) return;
	playerHealth->m_invincible = !playerHealth->m_invincible;
}

void ACTRL_Player::LoadNextCheckpoint()
{
	USYST_LogManager::PrintSpecial(this, "CHEAT: LoadNextCheckpoint called");

	m_teleporter->Teleport();
}

void ACTRL_Player::ToggleUnlockDirectionalShadowDash()
{
	USYST_LogManager::PrintSpecial(this, "CHEAT: UnlockDirectionalShadowDash called");

	isDirectionalShadowDashUnlocked = !isDirectionalShadowDashUnlocked;
}

void ACTRL_Player::ToggleUnlockVoidout()
{
	USYST_LogManager::PrintSpecial(this, "CHEAT: UnlockVoidout called");

	isVoidoutUnlocked = !isVoidoutUnlocked;
	UpdatePercentVoidout(voidoutCooldown, voidoutCooldown);
}

void ACTRL_Player::ForceSkip()
{
	USYST_LogManager::PrintSpecial(this, "CHEAT: ForceSkip called");

	if (GetGameInstance()->sequenceTrigger != nullptr)
	{
		GetGameInstance()->sequenceTrigger->SkipTheLevelSequence();
	}
}

#endif

void ACTRL_Player::CallTimer(float timeToMarkPlayer)
{
	SetMarkedByFlareShow(true);
	if(isShadowDashUnlocked)
	{
		ToggleUnlockShadowDash();
	}
	if (GetWorld()->GetTimerManager().IsTimerActive(m_timerHandler))
	{
		GetWorld()->GetTimerManager().ClearTimer(m_timerHandler);
	}
	GetWorld()->GetTimerManager().SetTimer(m_timerHandler, this, &ACTRL_Player::UpdateBlackBoardGlobal, timeToMarkPlayer, false);
}

void ACTRL_Player::UpdateBlackBoardGlobal()
{
	//SetWidgetShow(darkSlashIndicator->GetUserWidgetObject(), false);
	if (!isShadowDashUnlocked)
	{
		ToggleUnlockShadowDash();
	}
	BLKB_Global::seenByFlare(false);
	SetMarkedByFlareShow(false);
}