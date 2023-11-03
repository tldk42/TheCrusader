// Written by jaegang lim

#include "UI/Inventory/Player/PlayerItemSlot.h"

#include "Components/Image.h"
#include "Data/ItemDataStructs.h"
#include "Components/TextBlock.h"
#include "Item/Data/ItemBase.h"
#include "UI/Inventory/InventoryTooltip.h"

void UPlayerItemSlot::UpdateItemSlot(UItemBase* Item)
{
	if (!Item)
	{
		ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		GetToolTip()->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	ItemReference = Item;
	ItemIcon->SetBrushFromTexture(Item->AssetData.Icon);
	ItemIcon->SetVisibility(ESlateVisibility::Visible);
	UInventoryTooltip* InventoryTooltip = Cast<UInventoryTooltip>(GetToolTip());
	InventoryTooltip->InventorySlotBeingHovered = this;
	InventoryTooltip->UpdateToolTip();
	GetToolTip()->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);

	if (!ItemReference)
	{
		ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	}

	switch (SlotName)
	{
	case EEquipmentPart::Head:
		SlotNameText->SetText(FText::FromString("Head"));
		break;
	case EEquipmentPart::Torso:
		SlotNameText->SetText(FText::FromString("Torso"));
		break;
	case EEquipmentPart::Arm:
		SlotNameText->SetText(FText::FromString("Arm"));
		break;
	case EEquipmentPart::Legs:
		SlotNameText->SetText(FText::FromString("Pants"));
		break;
	case EEquipmentPart::Feet:
		SlotNameText->SetText(FText::FromString("Shoes"));
		break;
	case EEquipmentPart::Weapon:
		SlotNameText->SetText(FText::FromString("Weapon"));
		break;
	case EEquipmentPart::Shield:
		SlotNameText->SetText(FText::FromString("Shield"));
		break;
	case EEquipmentPart::Bow:
		SlotNameText->SetText(FText::FromString("Bow"));
		break;
	default: ;
	}

	GetToolTip()->SetVisibility(ESlateVisibility::Collapsed);
}

FReply UPlayerItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
