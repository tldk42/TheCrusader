// Written by jaegang lim


#include "Item/Data/ItemWeaponBase.h"

#include "Character/Balian.h"
#include "Component/Inventory/InventoryComponent.h"
#include "Item/Weapon/Item_Weapon.h"
#include "Item/Weapon/Item_Weapon_Bow.h"
#include "Kismet/KismetMathLibrary.h"


void UItemWeaponBase::MoveToEquipment(ABalian* Character)
{
	if (Character)
	{
		if (Character->GetInventory())
		{
			if (ItemData.WeaponData.Type == EWeaponType::Bow)
			{
			}
			else
			{
				if (AItem_Weapon* Weapon = Character->GetWorld()->SpawnActor<
					AItem_Weapon>(ItemData.AssetData.ItemToUse))
				{
					Character->GetInventory()->HandleEquipmentItem(this);

					Weapon->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					Weapon->DisableInteractionCollision();
					Weapon->SetInstigator(Character);
					Weapon->AttachToComponent(Character->GetMesh(),
					                          FAttachmentTransformRules::SnapToTargetIncludingScale,
					                          ItemData.EquipmentData.DetachmentSocket);
					Character->SetCurrentWeapon(Weapon);
				}
			}
		}
	}
}

AItem* UItemWeaponBase::Drop(const int32 NumToRemove)
{
	if (OwningInventory && OwningInventory->FindMatchingItem(this))
	{
		const AActor* Actor = OwningInventory->GetOwner();
		const FVector Start =
			Actor->GetActorLocation() +
			UKismetMathLibrary::RandomUnitVectorInConeInDegrees(
				Actor->GetActorForwardVector(), 30.f) * 150.f;
		const FVector End = Start - FVector(0, 0, 500.f);

		// DrawDebugLine(Actor->GetWorld(), Start, End, FColor::Red, true, 1.f, 0, 1.f);

		if (FHitResult HitResult; Actor->GetWorld()->
		                                 LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
		{
			const int32 RemovedQuantity = OwningInventory->RemoveAmountOfItem(this, NumToRemove);

			AItem_Weapon* WeaponItem;

			if (ItemData.WeaponData.Type == EWeaponType::Bow)
			{
				WeaponItem = Actor->GetWorld()->SpawnActor<AItem_Weapon_Bow>(
					ItemData.AssetData.ItemToUse, HitResult.Location, FRotator::ZeroRotator);
			}
			else
			{
				WeaponItem = Actor->GetWorld()->SpawnActor<AItem_Weapon>(
					ItemData.AssetData.ItemToUse, HitResult.Location, FRotator::ZeroRotator);
			}

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
			if (AItem_Weapon* Weapon = Cast<AItem_Weapon>(Drop(1)))
			{
				Character->GetInventory()->HandleEquipmentItem(this);

				Weapon->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				Weapon->DisableInteractionCollision();
				Weapon->SetInstigator(Character);
				Weapon->AttachToComponent(Character->GetMesh(),
				                          FAttachmentTransformRules::SnapToTargetIncludingScale,
				                          ItemData.EquipmentData.DetachmentSocket);
				if (ItemData.WeaponData.Type == EWeaponType::Bow)
				{
					Character->SetCurrentBow(Cast<AItem_Weapon_Bow>(Weapon));
				}
				else
				{
					Character->SetCurrentWeapon(Weapon);
				}

				Character->GetInventory()->OnInventoryUpdated.Broadcast();
			}
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
