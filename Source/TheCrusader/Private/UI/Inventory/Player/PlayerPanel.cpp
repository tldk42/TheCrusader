// Written by jaegang lim


#include "UI/Inventory/Player/PlayerPanel.h"

#include "Data/ItemDataStructs.h"
#include "UI/Inventory/Player/PlayerItemSlot.h"


void UPlayerPanel::RefreshInventory(const EEquipmentPart Part, UItemBase* ItemRef) const
{
	switch (Part)
	{
	case EEquipmentPart::Head:
		HeadSlot->UpdateItemSlot(ItemRef);
		break;
	case EEquipmentPart::Torso:
		TorsoSlot->UpdateItemSlot(ItemRef);
		break;
	case EEquipmentPart::Arm:
		ArmSlot->UpdateItemSlot(ItemRef);
		break;
	case EEquipmentPart::Pants:
		PantsSlot->UpdateItemSlot(ItemRef);
		break;
	case EEquipmentPart::Feet:
		FeetSlot->UpdateItemSlot(ItemRef);
		break;
	case EEquipmentPart::Weapon:
		WeaponSlot->UpdateItemSlot(ItemRef);
		break;
	case EEquipmentPart::Shield:
		ShieldSlot->UpdateItemSlot(ItemRef);
		break;
	case EEquipmentPart::Bow:
		BowSlot->UpdateItemSlot(ItemRef);
		break;
	default: ;
	}
}
