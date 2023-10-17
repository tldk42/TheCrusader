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
	// 플레이어에게 새로운 무기를 부착한다.
	if (Character)
	{
		/** Check List
		 * 1. 무기를 이미 장착중인가?
		 * 2. 장착중이지 않은가?
		 */

		// 1. 인벤토리에서 무기 슬롯을 확인한다. (인벤토리에서 처리)

		// Case 1 (장착중)
		// 2. 장착하려는 무기 = NewWeapon
		// 3. 기존 무기 = CurrentWeapon
		// 4. CurrentWeapon(Actor)은 월드에 이미 생성되어 있을것이다.
		// 5. Actor를 파괴하고 아이템의 정보(UItemWeaponBase)만을 다시 인벤토리에 저장해야한다.
		// 6. CurrentWeapon의 ItemRef를 가져와서 변수로 저장한다.
		// 7. CurrentWeapon을 파괴한다.
		// 8. New Weapon을 무기 Slot에 넣는다. (인벤토리에서 처리)

		if (Character->GetInventory())
		{
			Character->GetInventory()->HandleEquipmentItem(this);
		}

		// Case 2 (장착 X)
		// Case1의 8 - 10 


		// 캐릭터에 부착
		// 9. New Weapon을 월드에 Spawn한다.
		if (AItem_Weapon* Weapon = Cast<AItem_Weapon>(Drop(1)))
		{
			Weapon->SetInstigator(Character);
			Weapon->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Weapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			                          WeaponData.AttachmentSocket);
			// 10. New Weapon을 플레이어에게 부착및 장착(플레이어에서 처리)
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
