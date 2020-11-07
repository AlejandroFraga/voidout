
#include "USWG_AbilityBar.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SystemsManager.h"

#include "Components/ProgressBar.h"


void UUSWG_AbilityBar::NativeConstruct()
{
	Super::NativeConstruct();

	StoreWidgetAnimations();

	// We set the animations
	initialDisplacement = GetAnimationByName(TEXT("initialDisplacement"));
	shadowDashGlow = GetAnimationByName(TEXT("shadowDashGlow"));
	voidoutGlow = GetAnimationByName(TEXT("voidoutGlow"));
}

void UUSWG_AbilityBar::StoreWidgetAnimations()
{
	AnimationsMap.Empty();

	UProperty* Prop = GetClass()->PropertyLink;

	// check each property of this class
	while (Prop != nullptr)
	{
		// only evaluate object properties, skip rest
		if (Prop->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty* ObjProp = Cast<UObjectProperty>(Prop);

			// only get back properties that are of type widget animation
			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);
				// only get back properties that are of type widget animation
				UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(Obj);
				// if casting worked update map with new animation
				if (WidgetAnimation != nullptr && WidgetAnimation->MovieScene)
				{
					FName AnimName = WidgetAnimation->MovieScene->GetFName();
					AnimationsMap.Add(AnimName, WidgetAnimation);
				}
			}
		}
		Prop = Prop->PropertyLinkNext;
	}
}

UWidgetAnimation* UUSWG_AbilityBar::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnimation = AnimationsMap.Find(AnimationName);
	if (WidgetAnimation != nullptr)
		return *WidgetAnimation;

	return nullptr;
}

void UUSWG_AbilityBar::BeginStartLogoAnimation()
{
	if (initialDisplacement != nullptr)
		PlayAnimation(initialDisplacement);
	else
		USYST_LogManager::PrintError(this, "UUSWG_AbilityBar.BeginStartLogoAnimation: No initialDisplacement");
}

void UUSWG_AbilityBar::UpdatePercentShadowDash(float Value, float max_coolDown)
{
	if (max_coolDown != 0 && shadowDashProgressBar != nullptr)
		shadowDashProgressBar->SetPercent(GetCorrectedPercentShadowDash(Value / max_coolDown));
}

float UUSWG_AbilityBar::GetCorrectedPercentShadowDash(float Value)
{
	if(shadowDashProgressBarMax > shadowDashProgressBarMin)
		return (Value * (shadowDashProgressBarMax - shadowDashProgressBarMin)) + shadowDashProgressBarMin;

	return 0;
}

void UUSWG_AbilityBar::DoShadowDashGlow()
{
	if (shadowDashGlow != nullptr)
	{
		SYST_SystemsManager::GetSoundEventManager()->playAbilitySDCharged();
		PlayAnimation(shadowDashGlow);
	}
	else
		USYST_LogManager::PrintError(this, "UUSWG_AbilityBar.DoShadowDashGlow: No shadowDashGlow");
}

void UUSWG_AbilityBar::UpdatePercentVoidout(float Value, float max_coolDown)
{
	if (max_coolDown != 0 && voidoutProgressBar != nullptr)
		voidoutProgressBar->SetPercent(GetCorrectedPercentVoidout(Value / max_coolDown));
}

float UUSWG_AbilityBar::GetCorrectedPercentVoidout(float Value)
{
	if (voidoutProgressBarMax > voidoutProgressBarMin)
		return (Value * (voidoutProgressBarMax - voidoutProgressBarMin)) + voidoutProgressBarMin;

	return 0;
}

void UUSWG_AbilityBar::DoVoidoutGlow()
{
	if (voidoutGlow != nullptr)
	{
		SYST_SystemsManager::GetSoundEventManager()->playMenuSelection();
		PlayAnimation(voidoutGlow);
	}
	else
		USYST_LogManager::PrintError(this, "UUSWG_AbilityBar.DoVoidoutGlow: No voidoutGlow");
}
