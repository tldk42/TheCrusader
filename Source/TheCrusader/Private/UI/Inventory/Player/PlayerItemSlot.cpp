// Written by jaegang lim


#include "UI/Inventory/Player/PlayerItemSlot.h"

#include "Components/Image.h"
#include "Data/ItemDataStructs.h"
#include "Components/TextBlock.h"

void UPlayerItemSlot::UpdateItemSlot(UItemBase* Item)
{
	ItemReference = Item;
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
	case EEquipmentPart::Bow:
		SlotNameText->SetText(FText::FromString(L"원거리"));
		break;
	default: ;
	}
}

FReply UPlayerItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
