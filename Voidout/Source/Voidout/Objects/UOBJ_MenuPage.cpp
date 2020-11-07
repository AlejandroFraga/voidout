
#include "UOBJ_MenuPage.h"
#include "Objects/UOBJ_MenuButton.h"
#include "System/SYST_LogManager.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"


void UUOBJ_MenuPage::ShowWidget()
{
	if (widget != nullptr)
		widget->SetVisibility(ESlateVisibility::Visible);
}

void UUOBJ_MenuPage::HideWidget()
{
	if (widget != nullptr)
		widget->SetVisibility(ESlateVisibility::Hidden);
}

int UUOBJ_MenuPage::GetButtonPosition()
{
	if (menuButtons.Num() <= 0)
	{
		return 0;
	}
	if (m_buttonPosition < 0 || m_buttonPosition >= menuButtons.Num())
	{
		USYST_LogManager::PrintError(this, "UUOBJ_MenuPage.GetButtonPosition: m_buttonPosition corrected");
		m_buttonPosition = 0;
	}
	return m_buttonPosition;
}

void UUOBJ_MenuPage::SetButtonPosition(int buttonPosition)
{
	if (menuButtons.Num() <= 0)
	{
		return;
	}
	if (buttonPosition >= 0 && buttonPosition < menuButtons.Num())
		m_buttonPosition = buttonPosition;
	else
		USYST_LogManager::PrintError(this, "UUOBJ_MenuPage.SetButtonPosition: param buttonPosition out of bounds");
}

void UUOBJ_MenuPage::AddButton(UWidget* buttonWidget, UTextBlock* text, UButton* button)
{
	if (GetMenuButtonFromButton(button) != nullptr)
		return;

	UUOBJ_MenuButton* menuButton = NewObject<UUOBJ_MenuButton>();
	if (menuButton != nullptr)
	{
		if (text == nullptr)
			USYST_LogManager::PrintWarning(this, "UUSWG_MenuBase.AddPage: No text");
		menuButton->text = text;

		if (button == nullptr)
			USYST_LogManager::PrintWarning(this, "UUSWG_MenuBase.AddPage: No button");
		menuButton->button = button;

		menuButton->widget = buttonWidget;

		menuButtons.Add(menuButton);
	}
}

UUOBJ_MenuButton* UUOBJ_MenuPage::GetMenuButtonFromButton(UButton* button)
{
	if (button != nullptr)
	{
		for (int i = 0; i < menuButtons.Num(); i++)
		{
			if (menuButtons[i] != nullptr && menuButtons[i]->button == button)
				return menuButtons[i];
		}
	}
	return nullptr;
}