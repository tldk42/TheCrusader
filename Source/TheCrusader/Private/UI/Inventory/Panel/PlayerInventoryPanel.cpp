﻿// Written by jaegang lim


#include "UI/Inventory/Panel/PlayerInventoryPanel.h"

#include "Character/Balian.h"
#include "Component/Inventory/InventoryComponent.h"
#include "Data/ItemDataStructs.h"
#include "UI/Inventory/Player/PlayerItemSlot.h"


void UPlayerInventoryPanel::RefreshInventory()
{
	Super::RefreshInventory();

	HeadSlot->UpdateItemSlot(nullptr);
	TorsoSlot->UpdateItemSlot(nullptr);
	ArmSlot->UpdateItemSlot(nullptr);
	PantsSlot->UpdateItemSlot(nullptr);
	FeetSlot->UpdateItemSlot(nullptr);
	WeaponSlot->UpdateItemSlot(nullptr);
	ShieldSlot->UpdateItemSlot(nullptr);
	BowSlot->UpdateItemSlot(nullptr);


	for (const auto& InventoryItem : InventoryRef->GetEquippedContents())
	{
		switch (InventoryItem.Key)
		{
		case EEquipmentPart::Head:
			HeadSlot->UpdateItemSlot(InventoryItem.Value);
			break;
		case EEquipmentPart::Torso:
			TorsoSlot->UpdateItemSlot(InventoryItem.Value);
			break;
		case EEquipmentPart::Arm:
			ArmSlot->UpdateItemSlot(InventoryItem.Value);
			break;
		case EEquipmentPart::Pants:
			PantsSlot->UpdateItemSlot(InventoryItem.Value);
			break;
		case EEquipmentPart::Feet:
			FeetSlot->UpdateItemSlot(InventoryItem.Value);
			break;
		case EEquipmentPart::Weapon:
			WeaponSlot->UpdateItemSlot(InventoryItem.Value);
			break;
		case EEquipmentPart::Shield:
			ShieldSlot->UpdateItemSlot(InventoryItem.Value);
			break;
		case EEquipmentPart::Bow:
			BowSlot->UpdateItemSlot(InventoryItem.Value);
			break;
		default: ;
		}
	}
}

void UPlayerInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerRef = Cast<ABalian>(GetOwningPlayerPawn());

	if (PlayerRef)
	{
		InventoryRef = PlayerRef->GetInventory();
		if (InventoryRef)
		{
			InventoryRef->OnInventoryUpdated.AddUObject(this, &ThisClass::RefreshInventory);
			SetInfoText();
		}
	}
}
