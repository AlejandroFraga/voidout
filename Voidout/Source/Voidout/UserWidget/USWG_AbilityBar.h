
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "USWG_AbilityBar.generated.h"

UCLASS()
class VOIDOUT_API UUSWG_AbilityBar : public UUserWidget
{
	GENERATED_BODY()

	// ---------------------------------------------------------------------- VARIABLES.

	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UWidgetAnimation* initialDisplacement{ nullptr };

		/* SHADOW DASH */

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* shadowDashProgressBar{ nullptr };

	UWidgetAnimation* shadowDashGlow{ nullptr };

		/* VOIDOUT */

	UPROPERTY(meta = (BindWidget))
	UProgressBar* voidoutProgressBar { nullptr };

	UWidgetAnimation* voidoutGlow{ nullptr };

	// ---------------------------------------------------------------------- METHODS.

	void StoreWidgetAnimations();

	class UWidgetAnimation* GetAnimationByName(FName AnimationName) const;

		/* SHADOW DASH */

	float GetCorrectedPercentShadowDash(float Value);

		/* VOIDOUT */

	float GetCorrectedPercentVoidout(float Value);

protected:

	// ---------------------------------------------------------------------- VARIABLES.

		/* SHADOW DASH */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Shadow Dash", meta = (UIMin = '0', UMax = '1'))
	float shadowDashProgressBarMin{ 0.23f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Shadow Dash", meta = (UIMin = '0', UMax = '1'))
	float shadowDashProgressBarMax{ 0.68f };

		/* VOIDOUT */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Voidout", meta = (UIMin = '0', UMax = '1'))
	float voidoutProgressBarMin{ 0.23f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Voidout", meta = (UIMin = '0', UMax = '1'))
	float voidoutProgressBarMax{ 0.86f };

public:

	// ---------------------------------------------------------------------- METHODS.

	virtual void NativeConstruct() override;

	void BeginStartLogoAnimation();

		/* SHADOW DASH */

	void UpdatePercentShadowDash(float Value, float max_coolDown);

	void DoShadowDashGlow();

		/* VOIDOUT */

	void UpdatePercentVoidout(float Value, float max_coolDown);

	void DoVoidoutGlow();
};
