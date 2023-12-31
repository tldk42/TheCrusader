﻿// Written by jaegang lim


#include "Item/Data/ItemEquipmentBase.h"

#include "Character/Balian.h"
#include "Component/Inventory/InventoryComponent.h"
#include "Item/Equipment/Item_Equipment.h"


UItemEquipmentBase::UItemEquipmentBase()
{
	bIsPickup = true;
}

void UItemEquipmentBase::MoveToEquipment(ABalian* Character)
{
	if (Character)
	{
		if (Character->GetInventory())
		{
			Character->GetInventory()->HandleEquipmentItem(this);
			Character->AttachEquipment(ItemData.EquipmentData.EquipmentPart, this);
		}
	}
}

AItem* UItemEquipmentBase::Drop(int32 NumToRemove)
{
	// if (OwningInventory && OwningInventory->FindMatchingItem(this))
	// {
	// 	const AActor* Actor = OwningInventory->GetOwner();
	// 	const FVector Start =
	// 		Actor->GetActorLocation() +
	// 		UKismetMathLibrary::RandomUnitVectorInConeInDegrees(
	// 			Actor->GetActorForwardVector(), 30.f) * 150.f;
	// 	const FVector End = Start - FVector(0, 0, 500.f);
	//
	// 	// DrawDebugLine(Actor->GetWorld(), Start, End, FColor::Red, true, 1.f, 0, 1.f);
	//
	// 	if (FHitResult HitResult; Actor->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
	// 	{
	// 		const int32 RemovedQuantity = OwningInventory->RemoveAmountOfItem(this, NumToRemove);
	//
	// 		AItem_Equipment* Equipment = Actor->GetWorld()->SpawnActor<AItem_Equipment>(
	// 			AssetData.ItemClass, HitResult.Location, FRotator::ZeroRotator);
	//
	// 		Equipment->InitializeDrop(this, RemovedQuantity);
	//
	// 		return Equipment;
	// 	}
	// 	UE_LOG(LogTemp, Error, TEXT("Can not remove item from inventory"));
	// }
	return nullptr;
}

void UItemEquipmentBase::Use(ABalian* Character)
{
	if (Character)
	{
		if (Character->GetInventory())
		{
			Character->GetInventory()->HandleEquipmentItem(this);
			Character->AttachEquipment(ItemData.EquipmentData.EquipmentPart, this);
			OwningInventory->RemoveAmountOfItem(this, 1);
		}
	}
}

void UItemEquipmentBase::UnEquip(ABalian* Character)
{
	if (Character)
	{
		if (Character->GetInventory())
		{
			Character->DetachEquipment(ItemData.EquipmentData.EquipmentPart);
		}
	}
}
