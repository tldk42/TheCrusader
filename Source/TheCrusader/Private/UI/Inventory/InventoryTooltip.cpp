// Written by jaegang lim


#include "UI/Inventory/InventoryTooltip.h"

#include "Components/TextBlock.h"
#include "Data/ItemDataStructs.h"
#include "Item/Data/ItemBase.h"
#include "Item/Data/ItemWeaponBase.h"
#include "UI/Inventory/InventoryItemSlot.h"

void UInventoryTooltip::UpdateToolTip()
{
	if (InventorySlotBeingHovered)
	{
		if (const UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference())
		{
			switch (ItemBeingHovered->ItemData.ItemType)
			{
				{
				case EItemType::Armor:
					ItemType->SetText(UEnum::GetDisplayValueAsText(ItemBeingHovered->ItemData.ItemType));
					UsageText->SetVisibility(ESlateVisibility::Collapsed);
					MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
					StackWeight->SetVisibility(ESlateVisibility::Collapsed);
					break;
				}
				{
				case EItemType::Weapon:
					const UItemWeaponBase* Weapon = Cast<UItemWeaponBase>(ItemBeingHovered);
					ItemType->SetText(UEnum::GetDisplayValueAsText(Weapon->ItemData.WeaponData.Type));
					Power->SetText(FText::Format(
						FText::FromString(
							L"힘: {0}"), FText::AsNumber(Weapon->ItemData.WeaponData.BaseDamage)));
					UsageText->SetVisibility(ESlateVisibility::Collapsed);
					ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
					MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
					Armour->SetVisibility(ESlateVisibility::Collapsed);
					StackWeight->SetVisibility(ESlateVisibility::Collapsed);
					break;
				}
			case EItemType::Spell:
				break;
			case EItemType::Consumable:
				ItemType->SetText(FText::FromString("Consume"));
				DamageValue->SetVisibility(ESlateVisibility::Collapsed);
				ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
			// SellValue->SetVisibility(ESlateVisibility::Collapsed);
				break;
			case EItemType::Quest:
				break;
			case EItemType::Mundane:
				ItemType->SetText(FText::FromString("Normal"));
				DamageValue->SetVisibility(ESlateVisibility::Collapsed);
				ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
				UsageText->SetVisibility(ESlateVisibility::Collapsed);
			// SellValue->SetVisibility(ESlateVisibility::Collapsed);
				break;
			default: ;
			}

			ItemName->SetText(ItemBeingHovered->ItemData.TextData.Name);
			DamageValue->SetText(FText::Format(
				FText::FromString(
					L"손상도: {0}"), FText::AsNumber(ItemBeingHovered->ItemData.ItemStatistics.DamageValue)));
			UsageText->SetText(ItemBeingHovered->ItemData.TextData.UsageText);
			ItemDescription->SetText(ItemBeingHovered->ItemData.TextData.Description);
			SellValue->SetText(FText::Format(
				FText::FromString(
					L"판매가: {0}"), FText::AsNumber(ItemBeingHovered->ItemData.ItemStatistics.SellValue)));
			StackWeight->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight()));

			if (ItemBeingHovered->ItemData.NumericData.bIsStackable)
			{
				MaxStackSize->SetText(FText::AsNumber(ItemBeingHovered->ItemData.NumericData.MaxStackSize));
			}
			else
			{
				MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UInventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateToolTip();
}
