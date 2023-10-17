// Written by jaegang lim


#include "Item/Data/ItemBase.h"

#include "Component/Inventory/InventoryComponent.h"
#include "Data/ItemDataStructs.h"
#include "Item/Weapon/Item_Weapon.h"
#include "Kismet/KismetMathLibrary.h"

UItemBase::UItemBase()
	: bIsCopy(false),
	  bIsPickup(false)
{
}

void UItemBase::ResetItemFlags()
{
	bIsCopy = false;
	bIsPickup = false;
}

UItemBase* UItemBase::CreateItemCopy() const
{
	UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());

	ItemCopy->ID = this->ID;
	ItemCopy->Quantity = this->Quantity;
	ItemCopy->ItemQuality = this->ItemQuality;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->TextData = this->TextData;
	ItemCopy->NumericData = this->NumericData;
	ItemCopy->ItemStatistics = this->ItemStatistics;
	ItemCopy->AssetData = this->AssetData;
	ItemCopy->bIsCopy = true;

	return ItemCopy;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity == Quantity)
		return;

	Quantity = FMath::Clamp(NewQuantity, 0, NumericData.bIsStackable ? NumericData.MaxStackSize : 1);

	if (OwningInventory)
	{
		if (Quantity <= 0)
		{
			OwningInventory->RemoveSingleInstanceOfItem(this);
		}
	}
}

void UItemBase::Use(ABalian* Character)
{
}

AItemPickup* UItemBase::Drop(const int32 NumToRemove)
{
	if (OwningInventory && OwningInventory->FindMatchingItem(this))
	{
		const AActor* Actor = OwningInventory->GetOwner();
		const FVector Start =
			Actor->GetActorLocation() +
			UKismetMathLibrary::RandomUnitVectorInConeInDegrees(
				Actor->GetActorForwardVector(), 30.f) * 150.f;
		const FVector End = Start - FVector(0, 0, 500.f);

		if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
		{
			const int32 RemovedQuantity = OwningInventory->RemoveAmountOfItem(this, NumToRemove);

			AItemPickup* PickupItem = GetWorld()->SpawnActor<AItemPickup>(
				AssetData.ItemClass, HitResult.Location, FRotator::ZeroRotator);
			PickupItem->InitializeDrop(this, RemovedQuantity);
			return PickupItem;
		}
		UE_LOG(LogTemp, Error, TEXT("Can not remove item from inventory"));
	}
	return nullptr;
}

void UItemBase::UnEquip(ABalian* Character)
{
}
