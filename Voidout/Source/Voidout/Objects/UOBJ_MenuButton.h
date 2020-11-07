
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UOBJ_MenuButton.generated.h"

UCLASS()
class VOIDOUT_API UUOBJ_MenuButton : public UObject
{
	GENERATED_BODY()

public:

	/* VARIABLES */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button | Text")
	class UTextBlock* text{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	class UButton* button{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button | Widget")
	class UWidget* widget{ nullptr };

	/* FUNCTIONS */

	UFUNCTION(BlueprintCallable, Category = "Button | Widget")
	void ShowTextOutline();

	UFUNCTION(BlueprintCallable, Category = "Button | Widget")
	void HideTextOutline();

	UFUNCTION(BlueprintCallable, Category = "Button | Widget")
	void UpdateTextFont();

	UFUNCTION(BlueprintCallable, Category = "Button | Widget")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, Category = "Button | Widget")
	void HideWidget();
	
};
