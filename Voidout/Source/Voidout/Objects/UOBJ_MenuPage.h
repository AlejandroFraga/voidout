
#pragma once

#include "CoreMinimal.h"
#include "UOBJ_MenuPage.generated.h"

UCLASS()
class VOIDOUT_API UUOBJ_MenuPage : public UObject
{
	GENERATED_BODY()

public:

	/* VARIABLES */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page | Widget")
	class UWidget* widget{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page | Buttons")
	TArray<class UUOBJ_MenuButton*> menuButtons;

	/* FUNCTIONS */

	UFUNCTION(BlueprintCallable, Category = "Page | Widget")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, Category = "Page | Widget")
	void HideWidget();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Page | Buttons")
	int GetButtonPosition();

	UFUNCTION(BlueprintCallable, Category = "Page | Buttons")
	void SetButtonPosition(int buttonPosition);

	UFUNCTION(BlueprintCallable, Category = "Page | Widget")
	void AddButton(UWidget* buttonWidget, class UTextBlock* text, class UButton* button);

	UFUNCTION(BlueprintCallable, Category = "Page | Widget")
	UUOBJ_MenuButton* GetMenuButtonFromButton(UButton* button);

private:

	/* VARIABLES */

	int m_buttonPosition{ 0 };

};
