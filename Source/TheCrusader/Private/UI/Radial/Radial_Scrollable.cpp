// Written by jaegang lim


#include "UI/Radial/Radial_Scrollable.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void URadial_Scrollable::InitializeScrollable(const bool bInGamepad)
{
	bGamepad = bInGamepad;
}

void URadial_Scrollable::UpdatePage(const int CurrentPage, const int TotalPages) const
{
	Page_Text->SetText(
		FText::Format(FText::FromString("{0} / {1}"),
		              FText::AsNumber(CurrentPage + 1), FText::AsNumber(TotalPages)));
}

void URadial_Scrollable::ToggleVisibility(const bool bInGamepad) const
{
	const ESlateVisibility NewVisibility =
		bInGamepad ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;

	GamepadContainer_HorizontalBox->SetVisibility(NewVisibility);

	MouseIcon_Image->SetVisibility(NewVisibility);
}
