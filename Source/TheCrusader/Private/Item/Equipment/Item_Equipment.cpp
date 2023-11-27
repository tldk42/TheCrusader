// Written by jaegang lim


#include "Item/Equipment/Item_Equipment.h"


AItem_Equipment::AItem_Equipment()
	: bEquipped(false)
{
	PrimaryActorTick.bCanEverTick = false;
	ItemQuantity = 1;
}

void AItem_Equipment::BeginPlay()
{
	AActor::BeginPlay();

	if (!ItemRef)
	{
		InitializePickup(UItemEquipmentBase::StaticClass(), 1);
	}
}
