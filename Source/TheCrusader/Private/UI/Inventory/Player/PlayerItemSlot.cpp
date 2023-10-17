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
		SlotNameText->SetText(FText::FromString(L"머리"));
		break;
	case EEquipmentPart::Torso:
		SlotNameText->SetText(FText::FromString(L"상의"));
		break;
	case EEquipmentPart::Arm:
		SlotNameText->SetText(FText::FromString(L"팔"));
		break;
	case EEquipmentPart::Pants:
		SlotNameText->SetText(FText::FromString(L"하의"));
		break;
	case EEquipmentPart::Feet:
		SlotNameText->SetText(FText::FromString(L"신발"));
		break;
	case EEquipmentPart::Weapon:
		SlotNameText->SetText(FText::FromString(L"무기"));
		break;
	case EEquipmentPart::Shield:
		SlotNameText->SetText(FText::FromString(L"방패"));
		break;
	case EEquipmentPart::Bow:
		SlotNameText->SetText(FText::FromString(L"원거리"));
		break;
	default: ;
	}

	GetToolTip()->SetVisibility(ESlateVisibility::Collapsed);
}

FReply UPlayerItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
