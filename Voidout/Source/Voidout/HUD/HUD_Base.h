
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUD_Base.generated.h"

UCLASS()
class VOIDOUT_API AHUD_Base : public AHUD
{
	GENERATED_BODY()

	TSubclassOf<class UUserWidget> abilityBarClass;

	class UUSWG_AbilityBar* m_AbilityBar;

	TSubclassOf<UUserWidget> damageScreenClass;

	UUserWidget* m_DamageScreen;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// Cheats

	TSubclassOf<UUserWidget> cheatsHelpClass{ nullptr };

	UUserWidget* m_CheatsHelp{ nullptr };
#endif

	UUserWidget* CreateUserWidget(TSubclassOf<UUserWidget> widgetClass, bool show = true, int32 zOrder = 0);

	bool SetWidgetShow(UUserWidget* userWidget, bool show = true);

public:

	AHUD_Base();

	virtual void BeginPlay() override;

	bool ShowAll(bool show = true);

	UFUNCTION()
	void BeginStartLogoAnimation();

	void UpdatePercentShadowDash(float Value, float max_coolDown);

	void UpdatePercentVoidout(float Value, float max_coolDown);

	void DoShadowDashGlow();

	void DoVoidoutGlow();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// Cheats

	void ToggleShowCheatsHelp();
#endif
};
