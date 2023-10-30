// Written by jaegang lim


#include "Item/Equipment/Item_Equipment.h"


AItem_Equipment::AItem_Equipment()
	: bEquipped(false)
{
	PrimaryActorTick.bCanEverTick = false;
	ItemQuantity = 1;
}

void AItem_Equipment::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemEquipmentData* ItemData = ItemDataTable->FindRow<FItemEquipmentData>(
			DesiredItemID, DesiredItemID.ToString());
		if (ItemData)
		{
			UItemEquipmentBase* ItemBase = NewObject<UItemEquipmentBase>(this, UItemEquipmentBase::StaticClass());

			ItemBase->ID = ItemData->ItemID;
			ItemBase->ItemType = ItemData->ItemType;
			ItemBase->ItemQuality = ItemData->ItemQuality;
			ItemBase->ItemStatistics = ItemData->ItemStatistics;
			ItemBase->TextData = ItemData->TextData;
			ItemBase->NumericData = ItemData->NumericData;
			ItemBase->AssetData = ItemData->AssetData;
			ItemBase->EquipmentData = ItemData->EquipmentData;
			ItemBase->EquipmentPart = ItemData->EquipmentPart;
			ItemBase->bIsPickup = true;
			ItemBase->SetQuantity(InQuantity);

			ItemRef = ItemBase;

			Mesh->SetStaticMesh(ItemData->AssetData.Mesh);

			UpdateInteractableData();
		}
	}
}

void AItem_Equipment::BeginPlay()
{
	AActor::BeginPlay();

	if (!ItemRef)
	{
		InitializePickup(UItemEquipmentBase::StaticClass(), 1);
	}
}
