// Written by jaegang lim


#include "UI/Radial/Radial_Description.h"

#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void URadial_Description::NativeConstruct()
{
	Super::NativeConstruct();
}

void URadial_Description::UpdateDescription(const FText& NewDescription) const
{
	Description_Text->SetText(NewDescription);
}

void URadial_Description::InitializeDescription(const int DescriptionFontSize, const float DescriptionMaxWidth)
{
	FontSize = DescriptionFontSize;
	MaxWidth = DescriptionMaxWidth;

	// ReSharper disable once CppDeprecatedEntity
	Description_Text->Font.Size = FontSize;

	Container_SizeBox->SetWidthOverride(MaxWidth);
}
