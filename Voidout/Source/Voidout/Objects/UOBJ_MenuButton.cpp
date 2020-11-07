
#include "UOBJ_MenuButton.h"

#include "Components/TextBlock.h"
#include "Components/Widget.h"


void UUOBJ_MenuButton::ShowTextOutline()
{
	if (text != nullptr)
	{
		text->Font.OutlineSettings.OutlineColor.A = 1.f;
		UpdateTextFont();
	}
}

void UUOBJ_MenuButton::HideTextOutline()
{
	if (text != nullptr)
	{
		text->Font.OutlineSettings.OutlineColor.A = 0.f;
		UpdateTextFont();
	}
}

void UUOBJ_MenuButton::UpdateTextFont()
{
	if (text != nullptr)
		text->SetFont(text->Font);
}

void UUOBJ_MenuButton::ShowWidget()
{
	if (widget != nullptr)
		widget->SetVisibility(ESlateVisibility::Visible);
}

void UUOBJ_MenuButton::HideWidget()
{
	if (widget != nullptr)
		widget->SetVisibility(ESlateVisibility::Hidden);
}