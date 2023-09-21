// Written by jaegang lim


#include "Item/Data/ItemWeaponBase.h"

#include "Character/Balian.h"
#include "Component/Inventory/InventoryComponent.h"
#include "Item/Data/ItemBase.h"
#include "Item/Weapon/MyItem.h"

UItemBase* UItemWeaponBase::CreateItemCopy() const
{
	UItemWeaponBase* ItemCopy = NewObject<UItemWeaponBase>(StaticClass());

	ItemCopy->ID = this->ID;
	ItemCopy->Quantity = this->Quantity;
	ItemCopy->ItemQuality = this->ItemQuality;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->TextData = this->TextData;
	ItemCopy->NumericData = this->NumericData;
	ItemCopy->ItemStatistics = this->ItemStatistics;
	ItemCopy->AssetData = this->AssetData;
	ItemCopy->WeaponData = this->WeaponData;
	ItemCopy->bIsCopy = true;

	return ItemCopy;
}

void UItemWeaponBase::Use(ABalian* Character)
{
	
	
	if (Character)
	{
		// 플레이어 슬롯에 부착
		

		// 캐릭터에 부착
		AMyItem* Weapon = Character->GetWorld()->SpawnActor<AMyItem>(
			AMyItem::StaticClass());

		Character->GetInventory()->HandleEquipmentItem(this);
		
		Weapon->InitializeDrop(this, 1);
		Weapon->SetActorEnableCollision(false);
		Weapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
		                          WeaponData.AttachmentSocket);
		Character->SetCurrentWeapon(Weapon);

	}
}
