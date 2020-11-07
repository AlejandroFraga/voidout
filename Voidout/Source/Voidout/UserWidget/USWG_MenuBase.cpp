
#include "USWG_MenuBase.h"
#include "Controller/CTRL_Player.h"
#include "Objects/UOBJ_MenuPage.h"
#include "Objects/UOBJ_MenuButton.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SystemsManager.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


//FIXME To be deleted when the SYST_MenuManager is created
void UUSWG_MenuBase::OnStartGame()
{
	UWorld* world = GetWorld();
	if (world == nullptr)
		return;

	APlayerController* playerController = world->GetFirstPlayerController();
	if (playerController == nullptr)
		return;

	ACTRL_Player* ctrlPlayer = Cast<ACTRL_Player>(playerController);
	if (ctrlPlayer != nullptr)
	{
		ctrlPlayer->OnStartGame();
	}
}

void UUSWG_MenuBase::AttachToPlayer(int32 zOrder, bool showMenu, bool setActiveMenu, bool showMouseCursor, bool pauseGame)
{
	UWorld* world = GetWorld();
	if (world == nullptr)
		return;

	APlayerController* playerController = world->GetFirstPlayerController();
	if (playerController == nullptr)
		return;

	ACTRL_Player* ctrlPlayer = Cast<ACTRL_Player>(playerController);
	if (ctrlPlayer != nullptr)
		ctrlPlayer->AddMenu(this, zOrder, showMenu, setActiveMenu, showMouseCursor, pauseGame);
}

void UUSWG_MenuBase::DetachFromPlayer(bool resetActiveMenu, bool hideMouseCursor, bool resumeGame)
{
	UWorld* world = GetWorld();
	if (world == nullptr)
		return;

	APlayerController* playerController = world->GetFirstPlayerController();
	if (playerController == nullptr)
		return;

	ACTRL_Player* ctrlPlayer = Cast<ACTRL_Player>(playerController);
	if (ctrlPlayer != nullptr)
		ctrlPlayer->RemoveMenu(this, resetActiveMenu, hideMouseCursor, resumeGame);
}

void UUSWG_MenuBase::Show()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UUSWG_MenuBase::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}
//FIXME To be deleted when the SYST_MenuManager is created

void UUSWG_MenuBase::InitiateMenu(FName page)
{
	UpdatePage(page);
	UpdateButtonPosition(0);
}

void UUSWG_MenuBase::OpenSubMenu(UUSWG_MenuBase* _subActiveMenu)
{
	subActiveMenu = _subActiveMenu;
	if (subActiveMenu != nullptr)
	{
		subActiveMenu->parentMenu = this;
		subActiveMenu->AddToViewport(999);
	}
}

void UUSWG_MenuBase::CloseSubMenu()
{
	if (subActiveMenu != nullptr)
	{
		subActiveMenu->RemoveFromParent();
		subActiveMenu->parentMenu = nullptr;
	}
	subActiveMenu = nullptr;
}

UUOBJ_MenuPage* UUSWG_MenuBase::AddPage(FName page, UWidget* widget)
{
	if (!pages.Contains(page))
	{
		UUOBJ_MenuPage* menuPage = NewObject<UUOBJ_MenuPage>();

		if (widget == nullptr)
			USYST_LogManager::PrintWarning(this, "UUSWG_MenuBase.AddPage: No widget");
		if (menuPage != nullptr)
		{
			menuPage->widget = widget;
			pages.Add(page, menuPage);
		}

		return menuPage;
	}
	return GetPage(page);
}

void UUSWG_MenuBase::UpdatePage(FName page, bool restoreBtnPosBefore, bool restoreBtnPosAfter)
{
	if (subActiveMenu != nullptr)
	{
		subActiveMenu->UpdatePage(page, restoreBtnPosBefore, restoreBtnPosAfter);
		return;
	}

	if (!pageSelected.IsNone())
	{
		if(restoreBtnPosBefore)
			UpdateButtonPosition(0);

		RestorePage(pageSelected);
	}
	SetPage(page);
	SelectPage(page);
	RestoreAllButtonsPage();

	if(restoreBtnPosAfter)
		UpdateButtonPosition(0);
	else
		UpdateButtonPosition(GetPageButtonPosition());
}

void UUSWG_MenuBase::SetPage(FName page)
{
	if (subActiveMenu != nullptr)
	{
		subActiveMenu->SetPage(page);
		return;
	}

	if (pages.Contains(page))
		pageSelected = page;
}

UUOBJ_MenuPage* UUSWG_MenuBase::GetCurrentPage()
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->GetCurrentPage();

	return pages.FindRef(pageSelected);
}

UUOBJ_MenuPage* UUSWG_MenuBase::GetPage(FName pageName)
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->GetPage(pageName);

	return pages.FindRef(pageName);
}

int UUSWG_MenuBase::GetPageButtonPosition()
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->GetPageButtonPosition();

	if (GetCurrentPage() != nullptr)
		return GetCurrentPage()->GetButtonPosition();
	else
		USYST_LogManager::PrintError(this, "UUSWG_MenuBase.GetButtonPosition: No current page");
	return 0;
}

void UUSWG_MenuBase::SetPageButtonPosition(int buttonPosition)
{
	if (subActiveMenu != nullptr)
	{
		subActiveMenu->SetPageButtonPosition(buttonPosition);
		return;
	}

	if (GetCurrentPage() != nullptr)
		GetCurrentPage()->SetButtonPosition(buttonPosition);
	else
		USYST_LogManager::PrintError(this, "UUSWG_MenuBase.SetButtonPosition: No current page");
}

void UUSWG_MenuBase::AddButtonPosition(int add)
{
	if (subActiveMenu != nullptr)
	{
		subActiveMenu->AddButtonPosition(add);
		return;
	}

	UpdateButtonPosition(GetPageButtonPosition() + add);
}

bool UUSWG_MenuBase::UpdateButtonPosition(int position, bool playSoundIfChanged)
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->UpdateButtonPosition(position, playSoundIfChanged);

	const int buttonSelectedBefore = GetBoundedButtonPosition(GetPageButtonPosition());
	SetPageButtonPosition(GetBoundedButtonPosition(position));

	TArray<UUOBJ_MenuButton*> buttons = GetMenuButtonsPageSelected();
	if (buttons.Num() > 0)
	{
		RestoreMenuButton(buttons[buttonSelectedBefore]);
		SelectMenuButton(buttons[GetPageButtonPosition()]);
	}

	bool hasButtonChanged = buttonSelectedBefore != GetPageButtonPosition();
	if (playSoundIfChanged && hasButtonChanged)
	{
		PlayButtonMenuMoveSound();
	}
	return hasButtonChanged;
}

bool UUSWG_MenuBase::UpdateButtonPositionByButton(UButton* button, bool playSoundIfChanged)
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->UpdateButtonPositionByButton(button, playSoundIfChanged);

	if (button != nullptr)
	{
		const int buttonSelectedBefore = GetBoundedButtonPosition(GetPageButtonPosition());
		SetPageButtonPosition(GetButtonsPageSelected().Find(button));

		TArray<UUOBJ_MenuButton*> buttons = GetMenuButtonsPageSelected();
		if (GetPageButtonPosition() >= 0 && buttons.Num() > 0)
		{
			RestoreMenuButton(buttons[buttonSelectedBefore]);
			SelectMenuButton(GetCurrentPage()->GetMenuButtonFromButton(button));
		}
		else
			USYST_LogManager::PrintError(this, "UUSWG_MenuBase.UpdateButtonPositionByButton: El boton no esta en la pagina");

		bool hasButtonChanged = buttonSelectedBefore != GetPageButtonPosition();
		if (playSoundIfChanged && hasButtonChanged)
		{
			PlayButtonMenuMoveSound();
		}
		return hasButtonChanged;
	}
	return false;
}

bool UUSWG_MenuBase::UpdateButtonPositionByMenuButton(UUOBJ_MenuButton* button, bool playSoundIfChanged)
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->UpdateButtonPositionByMenuButton(button, playSoundIfChanged);

	if(button != nullptr)
	{
		const int buttonSelectedBefore = GetBoundedButtonPosition(GetPageButtonPosition());
		SetPageButtonPosition(GetMenuButtonsPageSelected().Find(button));

		TArray<UUOBJ_MenuButton*> buttons = GetMenuButtonsPageSelected();
		if (GetPageButtonPosition() >= 0 && buttons.Num() > 0)
		{
			RestoreMenuButton(buttons[buttonSelectedBefore]);
			SelectMenuButton(button);
		}
		else
			USYST_LogManager::PrintError(this, "UUSWG_MenuBase.UpdateButtonPositionByButton: El botón no está en la página");

		bool hasButtonChanged = buttonSelectedBefore != GetPageButtonPosition();
		if (playSoundIfChanged && hasButtonChanged)
		{
			PlayButtonMenuMoveSound();
		}
		return hasButtonChanged;
	}
	return false;
}

void UUSWG_MenuBase::RestoreAllButtonsPage()
{
	if (subActiveMenu != nullptr)
	{
		subActiveMenu->RestoreAllButtonsPage();
		return;
	}

	TArray<UUOBJ_MenuButton*> buttons = GetMenuButtonsPageSelected();
	if (buttons.Num() > 0)
	{
		for (int i = 0; i < buttons.Num(); ++i)
		{
			RestoreMenuButton(buttons[i]);
		}
	}
}

int UUSWG_MenuBase::GetBoundedButtonPosition(int position) const
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->GetBoundedButtonPosition(position);

	if (position <= 0)
		return 0;
	
	if (position < GetButtonsPageSelected().Num())
	{
		return position;
	}
	return GetButtonsPageSelected().Num() - 1;
}

TArray<UButton*> UUSWG_MenuBase::GetButtonsPageSelected() const
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->GetButtonsPageSelected();

	TArray<UUOBJ_MenuButton*> buttonsPage = GetButtonsPage(pageSelected);
	TArray<UButton*> buttons = TArray<UButton*>();

	for (int i = 0; i < buttonsPage.Num(); i++)
	{
		if(buttonsPage[i] != nullptr)
			buttons.Add(buttonsPage[i]->button);
	}

	return buttons;
}

TArray<UUOBJ_MenuButton*> UUSWG_MenuBase::GetMenuButtonsPageSelected() const
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->GetMenuButtonsPageSelected();

	return GetButtonsPage(pageSelected);
}

TArray<UUOBJ_MenuButton*> UUSWG_MenuBase::GetButtonsPage(FName pageName) const
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->GetButtonsPage(pageName);

	UUOBJ_MenuPage* page = pages.FindRef(pageName);
	if (page == nullptr)
		return TArray<UUOBJ_MenuButton*>();

	return page->menuButtons;
}

UUOBJ_MenuButton* UUSWG_MenuBase::GetButtonSelected()
{
	if (subActiveMenu != nullptr)
		return subActiveMenu->GetButtonSelected();

	TArray<UUOBJ_MenuButton*> buttons = GetButtonsPage(pageSelected);
	if(buttons.Num() <= 0)
		return nullptr;

	return buttons[GetBoundedButtonPosition(GetPageButtonPosition())];
}

void UUSWG_MenuBase::PlayButtonMenuMoveSound()
{
	SYST_SystemsManager::GetSoundEventManager()->playMenuMovement();
}

void UUSWG_MenuBase::PlayButtonMenuSelectSound()
{
	SYST_SystemsManager::GetSoundEventManager()->playMenuBack();
}

void UUSWG_MenuBase::PlayButtonMenuStartSound()
{
	SYST_SystemsManager::GetSoundEventManager()->playMenuStartButton();
}