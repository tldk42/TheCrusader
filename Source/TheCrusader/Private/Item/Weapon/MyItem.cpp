// Written by jaegang lim


#include "Item/Weapon/MyItem.h"


AMyItem::AMyItem()
	: bEquipped(false)
{
	PrimaryActorTick.bCanEverTick = false;
	ItemQuantity = 1;
}

void AMyItem::BeginPlay()
{
	AActor::BeginPlay();
	InitializePickup(UItemWeaponBase::StaticClass(), 1);
}

void AMyItem::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemWeaponData* ItemData = ItemDataTable->FindRow<FItemWeaponData>(
			DesiredItemID, DesiredItemID.ToString());
		if (ItemData)
		{
			ItemRef = NewObject<UItemWeaponBase>(this, BaseClass);

			ItemRef->ID = ItemData->ItemID;
			ItemRef->ItemType = ItemData->ItemType;
			ItemRef->ItemQuality = ItemData->ItemQuality;
			ItemRef->ItemStatistics = ItemData->ItemStatistics;
			ItemRef->TextData = ItemData->TextData;
			ItemRef->NumericData = ItemData->NumericData;
			ItemRef->AssetData = ItemData->AssetData;
			Cast<UItemWeaponBase>(ItemRef)->WeaponData = ItemData->WeaponData;
			ItemRef->bIsPickup = true;

			InQuantity <= 0 ? ItemRef->SetQuantity(1) : ItemRef->SetQuantity(InQuantity);

			PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);

			UpdateInteractableData();
		}
	}
}
