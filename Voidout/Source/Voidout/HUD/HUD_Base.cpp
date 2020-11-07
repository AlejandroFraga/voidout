
#include "HUD_Base.h"
#include "UserWidget/USWG_AbilityBar.h"
#include "System/SYST_LogManager.h"

#include "Components/WidgetComponent.h"
#include "Components/Image.h"
#include "UObject/ConstructorHelpers.h"

AHUD_Base::AHUD_Base()
{
	static ConstructorHelpers::FClassFinder<UUSWG_AbilityBar> USWG_AbilityBar_BP_Class(TEXT("/Game/Blueprints/UserWidget/HUD/USWG_AbilityBar_BP"));
	abilityBarClass = USWG_AbilityBar_BP_Class.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> USWG_DamageScreen_Class(TEXT("/Game/Blueprints/UserWidget/HUD/USWG_DamageScreen"));
	damageScreenClass = USWG_DamageScreen_Class.Class;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	static ConstructorHelpers::FClassFinder<UUserWidget> USWG_CheatsHelp_Class(TEXT("/Game/Blueprints/UserWidget/HUD/USWG_CheatsHelp"));
	cheatsHelpClass = USWG_CheatsHelp_Class.Class;
#endif
}

void AHUD_Base::BeginPlay()
{
	Super::BeginPlay();

	m_AbilityBar = Cast<UUSWG_AbilityBar>(CreateUserWidget(abilityBarClass, true, 1));
	m_DamageScreen = CreateUserWidget(damageScreenClass);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	m_CheatsHelp = CreateUserWidget(cheatsHelpClass, false);
#endif
}

UUserWidget* AHUD_Base::CreateUserWidget(TSubclassOf<UUserWidget> widgetClass, bool show, int32 zOrder)
{
	if (widgetClass != nullptr)
	{
		UUserWidget* userWidget = CreateWidget<UUserWidget>(GetWorld(), widgetClass);
		if(userWidget != nullptr)
			userWidget->AddToViewport(zOrder);
		SetWidgetShow(userWidget, show);
		return userWidget;
	}
	return nullptr;
}

bool AHUD_Base::SetWidgetShow(UUserWidget* userWidget,  bool show)
{
	if (userWidget != nullptr)
	{
		bool visibleBefore = userWidget->IsVisible();
		userWidget->SetVisibility(show ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		return visibleBefore;
	}
	return false;
}

bool AHUD_Base::ShowAll(bool show)
{
	bool visibleBefore = SetWidgetShow(m_AbilityBar, show);
	visibleBefore &= SetWidgetShow(m_DamageScreen, show);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if(m_CheatsHelp != nullptr && m_CheatsHelp->IsVisible() && !show)
		SetWidgetShow(m_CheatsHelp, false);
#endif

	return visibleBefore;
}

void AHUD_Base::BeginStartLogoAnimation()
{
	if (m_AbilityBar != nullptr)
	{
		m_AbilityBar->BeginStartLogoAnimation();
	}
}

void AHUD_Base::UpdatePercentShadowDash(float Value, float max_coolDown)
{
	if (m_AbilityBar != nullptr)
	{
		m_AbilityBar->UpdatePercentShadowDash(Value, max_coolDown);
	}
}

void AHUD_Base::UpdatePercentVoidout(float Value, float max_coolDown)
{
	if (m_AbilityBar != nullptr)
	{
		m_AbilityBar->UpdatePercentVoidout(Value, max_coolDown);
	}
}

void AHUD_Base::DoShadowDashGlow()
{
	if (m_AbilityBar != nullptr)
	{
		m_AbilityBar->DoShadowDashGlow();
	}
	else
	{
		USYST_LogManager::PrintError(this, "AHUD_Base.DoShadowDashGlow: No m_AbilityBar");
	}
}

void AHUD_Base::DoVoidoutGlow()
{
	if (m_AbilityBar != nullptr)
	{
		m_AbilityBar->DoVoidoutGlow();
	}
	else
	{
		USYST_LogManager::PrintError(this, "AHUD_Base.DoShadowDashGlow: No m_AbilityBar");
	}
}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
void AHUD_Base::ToggleShowCheatsHelp()
{
	if (m_CheatsHelp != nullptr)
	{
		SetWidgetShow(m_CheatsHelp, !m_CheatsHelp->IsVisible());
	}
	else
	{
		USYST_LogManager::PrintError(this, "AHUD_Base.ToggleShowCheatsHelp: No m_CheatsHelp");
	}
}
#endif
