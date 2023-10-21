// Written by jaegang lim


#include "Item/Data/ItemWeaponBase.h"

#include "Character/Balian.h"
#include "Component/Inventory/InventoryComponent.h"
#include "Item/Weapon/Item_Weapon.h"
#include "Kismet/KismetMathLibrary.h"

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

AItemPickup* UItemWeaponBase::Drop(int32 NumToRemove)
{
	if (OwningInventory && OwningInventory->FindMatchingItem(this))
	{
		const AActor* Actor = OwningInventory->GetOwner();
		const FVector Start =
			Actor->GetActorLocation() +
			UKismetMathLibrary::RandomUnitVectorInConeInDegrees(
				Actor->GetActorForwardVector(), 30.f) * 150.f;
		const FVector End = Start - FVector(0, 0, 500.f);

		DrawDebugLine(Actor->GetWorld(), Start, End, FColor::Red, true, 1.f, 0, 1.f);

		if (FHitResult HitResult; Actor->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
		{
			const int32 RemovedQuantity = OwningInventory->RemoveAmountOfItem(this, NumToRemove);

			AItem_Weapon* WeaponItem = Actor->GetWorld()->SpawnActor<AItem_Weapon>(
				AssetData.ItemClass, HitResult.Location, FRotator::ZeroRotator);

			WeaponItem->InitializeDrop(this, RemovedQuantity);

			return WeaponItem;
		}
		UE_LOG(LogTemp, Error, TEXT("Can not remove item from inventory"));
	}
	return nullptr;
}

void UItemWeaponBase::Use(ABalian* Character)
{
	if (Character)
	{
		if (Character->GetInventory())
		{
			Character->GetInventory()->HandleEquipmentItem(this);
		}

	
		if (AItem_Weapon* Weapon = Cast<AItem_Weapon>(Drop(1)))
		{
			Weapon->SetInstigator(Character);
			Weapon->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Weapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			                          WeaponData.AttachmentSocket);
			Character->SetCurrentWeapon(Weapon);
		}
	}
}

void UItemWeaponBase::UnEquip(ABalian* Character)
{
	if (Character)
	{
		if (Character->GetInventory())
		{
			Character->GetInventory()->RemoveEquipmentItem(EEquipmentPart::Weapon);
		}

		Character->SetCurrentWeapon(nullptr);
	}
}
