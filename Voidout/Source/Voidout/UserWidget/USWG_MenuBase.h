
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "USWG_MenuBase.generated.h"

UCLASS()
class VOIDOUT_API UUSWG_MenuBase : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Pages")
	TMap<FName, class UUOBJ_MenuPage*> pages = TMap<FName, UUOBJ_MenuPage*>();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Pages")
	FName pageSelected{};

	//FIXME To be deleted when the SYST_MenuManager is created
	UFUNCTION(BlueprintCallable, Category = "Menu | Menu")
	void OnStartGame();

	UFUNCTION(BlueprintCallable, Category = "Menu | Menu")
	void AttachToPlayer(int32 zOrder, bool showMenu = true, bool setActiveMenu = true, bool showMouseCursor = true, bool pauseGame = true);

	UFUNCTION(BlueprintCallable, Category = "Menu | Menu")
	void DetachFromPlayer(bool resetActiveMenu = true, bool hideMouseCursor = true, bool resumeGame = true);
	//FIXME To be deleted when the SYST_MenuManager is created

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Menu")
	void Constructor();

	UFUNCTION(BlueprintCallable, Category = "Menu | Menu")
	void Show();

	UFUNCTION(BlueprintCallable, Category = "Menu | Menu")
	void Hide();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Menu")
	void Back();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Menu")
	void Right();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Menu")
	void Left();

	UFUNCTION(BlueprintCallable, Category = "Menu | Menu")
	void InitiateMenu(FName page = "main");

	UFUNCTION(BlueprintCallable, Category = "Menu | SubMenu")
	void OpenSubMenu(UUSWG_MenuBase* _subActiveMenu);

	UFUNCTION(BlueprintCallable, Category = "Menu | SubMenu")
	void CloseSubMenu();

	UFUNCTION(BlueprintCallable, Category = "Menu | Pages")
	UUOBJ_MenuPage* AddPage(FName page, UWidget* widget);

	UFUNCTION(BlueprintCallable, Category = "Menu | Pages")
	void UpdatePage(FName page, bool restoreBtnPosBefore = true, bool restoreBtnPosAfter = true);

	UFUNCTION(BlueprintCallable, Category = "Menu | Pages")
	UUOBJ_MenuPage* GetCurrentPage();

	UFUNCTION(BlueprintCallable, Category = "Menu | Pages")
	UUOBJ_MenuPage* GetPage(FName pageName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Pages")
	void RestorePage(FName page);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Pages")
	void SelectPage(FName page);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Pages")
	void NextPage();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Pages")
	void PreviousPage();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Menu | Buttons")
	int GetPageButtonPosition();

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	void SetPageButtonPosition(int buttonPosition);

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	void AddButtonPosition(int add);

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	bool UpdateButtonPosition(int position, bool playSoundIfChanged = true);

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	bool UpdateButtonPositionByButton(UButton* button, bool playSoundIfChanged = true);

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	bool UpdateButtonPositionByMenuButton(UUOBJ_MenuButton* button, bool playSoundIfChanged = true);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Buttons")
	UButton* RestoreButton(UButton* button);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Buttons")
	UWidget* RestoreWidget(UWidget* widget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Buttons")
	UUOBJ_MenuButton* RestoreMenuButton(UUOBJ_MenuButton* menuButton);

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	void RestoreAllButtonsPage();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Buttons")
	UButton* SelectButton(UButton* button);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Buttons")
	UWidget* SelectWidget(UWidget* widget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Buttons")
	UUOBJ_MenuButton* SelectMenuButton(UUOBJ_MenuButton* menuButton);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu | Buttons")
	void PressButton();

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	TArray<UButton*> GetButtonsPageSelected() const;

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	TArray<UUOBJ_MenuButton*> GetMenuButtonsPageSelected() const;

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	TArray<UUOBJ_MenuButton*> GetButtonsPage(FName pageName) const;

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	UUOBJ_MenuButton* GetButtonSelected();

protected:
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Pages")
	UUSWG_MenuBase* subActiveMenu{ nullptr };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Pages")
	UUSWG_MenuBase* parentMenu{ nullptr };

private:

	UFUNCTION(BlueprintCallable, Category = "Menu | Pages")
	void SetPage(FName page);

	UFUNCTION(BlueprintCallable, Category = "Menu | Buttons")
	int GetBoundedButtonPosition(int position) const;

	UFUNCTION(BlueprintCallable, Category = "Menu | Sounds")
	void PlayButtonMenuMoveSound();

	UFUNCTION(BlueprintCallable, Category = "Menu | Sounds")
	void PlayButtonMenuSelectSound();

	UFUNCTION(BlueprintCallable, Category = "Menu | Sounds")
	void PlayButtonMenuStartSound();
};
