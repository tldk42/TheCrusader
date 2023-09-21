// Written by jaegang lim


#include "UI/Inventory/InventoryTooltip.h"

#include "Components/TextBlock.h"
#include "Data/ItemDataStructs.h"
#include "Item/Data/ItemBase.h"
#include "UI/Inventory/InventoryItemSlot.h"

void UInventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	if (InventorySlotBeingHovered)
	{
		const UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference();
		if (ItemBeingHovered)
		{

			switch (ItemBeingHovered->ItemType)
			{
			case EItemType::Armor:
				break;
			case EItemType::Weapon:
				ItemType->SetText(FText::FromString("Weapon"));
				UsageText->SetVisibility(ESlateVisibility::Collapsed);
				break;
			case EItemType::Shield:
				break;
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

			ItemName->SetText(ItemBeingHovered->TextData.Name);
			DamageValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.DamageValue));
			UsageText->SetText(ItemBeingHovered->TextData.UsageText);
			ItemDescription->SetText(ItemBeingHovered->TextData.Description);
			SellValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.SellValue));
			StackWeight->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight()));

			if (ItemBeingHovered->NumericData.bIsStackable)
			{
				MaxStackSize->SetText(FText::AsNumber(ItemBeingHovered->NumericData.MaxStackSize));
			}
			else
			{
				MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		
	}
}
