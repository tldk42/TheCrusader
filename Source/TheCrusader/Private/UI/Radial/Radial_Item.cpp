// Written by jaegang lim


#include "UI/Radial/Radial_Item.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void URadial_Item::InitializeItem()
{
	if (IsValid(Material))
	{
		SetMaterial();

		if (IsValid(Item.Icon))
		{
			Icon_Image->SetDesiredSizeOverride(FVector2D(IconSize, IconSize));
			Icon_Image->SetBrushFromTexture(Item.Icon);

			SetItemNameText();

			switch (Item.ButtonType)
			{
			case ERadialMenu_ButtonType::SubMenu:
				SetSubMenuIndicator();
				break;
			case ERadialMenu_ButtonType::RingMenu:
				CheckForSubItems();
				break;
			default: ;
			}
		}
		else
		{
			Icon_Image->SetBrushFromTexture(DefaultIcon);
			ItemName_Text->RemoveFromParent();
		}
	}
	else
	{
		Icon_Image->SetDesiredSizeOverride(FVector2D(OuterRingIconSize, OuterRingIconSize));
		Icon_Image->SetBrushFromTexture(Item.Icon);
	}

	CoolTime->SetVisibility(Item.ItemType == ERadialMenu_ItemType::Skill
		                        ? ESlateVisibility::Visible
		                        : ESlateVisibility::Collapsed);
}

void URadial_Item::SetMaterial()
{
	Background_Image->SetDesiredSizeOverride(FVector2D(MenuSize, MenuSize));

	BackgroundMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), Material);

	Background_Image->SetBrushFromMaterial(BackgroundMaterial);

	BackgroundMaterial->SetScalarParameterValue(FName("Sectors"), Sectors);
	BackgroundMaterial->SetScalarParameterValue(FName("Selection"), SectorIndex);
}

void URadial_Item::SetItemNameText() const
{
	// ReSharper disable once CppDeprecatedEntity
	ItemName_Text->Font.Size = FontSize;

	ItemName_Text->SetText(Item.DisplayName);
}

void URadial_Item::CheckForSubItems() const
{
	FString ItemName = Item.DisplayName.ToString();
	ItemName = ItemName.Replace(TEXT(" "), TEXT(""));

	const FRadialMenuData* SubItems = DT_Menu->FindRow<FRadialMenuData>(FName(*ItemName), TEXT("NO FOUND"));

	if (Item.ButtonType == ERadialMenu_ButtonType::RingMenu)
	{
		const float ItemNum = SubItems->Items.Num();
		const float SectorAngle = 360 / Sectors;
		const float ItemTheta = SectorAngle / ItemNum;
		CreateItems(*SubItems, SectorAngle, ItemTheta, ItemNum);
	}
}

FVector2D URadial_Item::CalculateTranslation(const int Index, const float SectorAngle, const float ItemTheta,
                                             const float ItemNum) const
{
	const float TempRad = ((SectorIndex * SectorAngle - 90.f) + Index * ItemTheta) - (ItemNum * ItemTheta / 2) +
		ItemTheta / 2;
	const FVector2D NormalizedRad = FVector2D(UKismetMathLibrary::DegCos(TempRad), UKismetMathLibrary::DegSin(TempRad));
	return NormalizedRad * OuterRingDistanceFromCenter;
}

void URadial_Item::SetSubMenuIndicator() const
{
	FString SubMenuText = Item.DisplayName.ToString();
	SubMenuText.Append("...");
	ItemName_Text->SetText(FText::FromString(SubMenuText));
}
