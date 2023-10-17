// Written by jaegang lim

#include "Component/Inventory/InventoryComponent.h"

#include "Item/Data/ItemBase.h"
#include "Item/Data/ItemEquipmentBase.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (InventoryContents.Contains(ItemIn))
		{
			return ItemIn;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByKey(ItemIn))
		{
			return *Result;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* ItemIn) const
{
	// FindByPredict는 Pred에 일치하는 첫 요소를 반환함.
	// InventoryContents에서 Iterating하면서 탐색 
	if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result =
		InventoryContents.FindByPredicate([&ItemIn](const UItemBase* InventoryItem) // InventoryItem -> *It
		{
			return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullItemStack();
		}))
	{
		return *Result;
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindMatchingEquipmentSlot(const EEquipmentPart Part) const
{
	if (EquippedContents.Contains(Part))
	{
		return EquippedContents[Part];
	}
	return nullptr;
}

int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt(
		(GetWeightCapacity() - InventoryTotalWeight) / ItemIn->GetItemSingleWeight());

	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		return RequestedAddAmount;
	}
	return WeightMaxAddAmount;
}

int32 UInventoryComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 RequestedAddAmount)
{
	const int32 AddAmountToMakeFullStack = StackableItem->NumericData.MaxStackSize - StackableItem->Quantity;

	return FMath::Min(RequestedAddAmount, AddAmountToMakeFullStack);
}

void UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* ItemToRemove)
{
	InventoryContents.RemoveSingle(ItemToRemove);
	OnInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemToRemove, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemToRemove->Quantity);

	ItemToRemove->SetQuantity(ItemToRemove->Quantity - ActualAmountToRemove);

	InventoryTotalWeight -= ActualAmountToRemove * ItemToRemove->GetItemSingleWeight();

	OnInventoryUpdated.Broadcast();

	return ActualAmountToRemove;
}

void UInventoryComponent::SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit)
{
	if (InventoryContents.Num() + 1 <= InventorySlotCapacity)
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}

void UInventoryComponent::RemoveEquipmentItem(EEquipmentPart Part)
{
	EquippedContents.Remove(Part);
}

int32 UInventoryComponent::HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	return 0;
}

FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	if (FMath::IsNearlyZero(ItemIn->GetItemSingleWeight()) || ItemIn->GetItemSingleWeight() < 0)
	{
		// return added none
		return FItemAddResult::AddedNone(
			FText::Format(FText::FromString("Could not add {0} to the inventory. Item has invalid weight value."),
			              ItemIn->TextData.Name));
	}

	// overflow weight
	if (InventoryTotalWeight + ItemIn->GetItemSingleWeight() > GetWeightCapacity())
	{
		// return added none
		return FItemAddResult::AddedNone(
			FText::Format(FText::FromString("Could not add {0} to the inventory. Item has overflow weight."),
			              ItemIn->TextData.Name));
	}

	// overflow slot
	if (InventoryContents.Num() + 1 > InventorySlotCapacity)
	{
		// return added none
		return FItemAddResult::AddedNone(
			FText::Format(FText::FromString("Could not add {0} to the inventory. Item has overflow slot."),
			              ItemIn->TextData.Name));
	}

	AddNewItem(ItemIn, RequestedAddAmount);

	// return added all result
	return FItemAddResult::AddedAll(
		FText::Format(FText::FromString("Successfully added {0} {1} to the inventory."),
		              RequestedAddAmount, ItemIn->TextData.Name), RequestedAddAmount);
}

void UInventoryComponent::HandleEquipmentItem(UItemBase* ItemToAdd)
{
	/** Equipment 종류는 3가지 뿐
	 * 1. 무기
	 * 2. 활
	 * 3. 방어구
	 */

	// 2. 장착하려는 무기 = NewWeapon
	// 3. 기존 무기 = CurrentWeapon
	// 4. CurrentWeapon(Actor)은 월드에 이미 생성되어 있을것이다 (플레이어 소유).
	// 5. Actor를 파괴하고 아이템의 정보(UItemWeaponBase)만을 다시 인벤토리에 저장해야한다.
	// 6. CurrentWeapon의 ItemRef를 가져와서 변수로 저장한다.
	// 7. CurrentWeapon을 파괴한다.
	// 8. New Weapon을 무기 Slot에 넣는다. (인벤토리에서 처리)
	// 9. New Weapon을 월드에 Spawn한다.
	// 10. New Weapon을 플레이어에게 부착및 장착(플레이어에서 처리)
	if (GetOwner())
	{
		if (ItemToAdd->ItemType == EItemType::Weapon)
		{
			EquippedContents.Add(EEquipmentPart::Weapon, ItemToAdd);
		}
		else
		{
			UItemEquipmentBase* Equipment = Cast<UItemEquipmentBase>(ItemToAdd);
			EquippedContents.Add(Equipment->EquipmentPart, ItemToAdd);
		}
	}
}

void UInventoryComponent::AttachEquipmentItem(EEquipmentPart Part)
{
}

void UInventoryComponent::DettachEquipmentItem(EEquipmentPart Part)
{
	UItemBase* ItemRef = EquippedContents[Part];
	EquippedContents.Remove(Part);
	HandleAddItem(ItemRef);
}

FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* InputItem)
{
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;

		if (!InputItem->NumericData.bIsStackable)
		{
			return HandleNonStackableItems(InputItem, InitialRequestedAddAmount);
		}

		const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			// return all result
			return FItemAddResult::AddedAll(FText::Format(
				                                FText::FromString("Successfully added {0} {1} to the inventory."),
				                                InitialRequestedAddAmount,
				                                InputItem->TextData.Name),
			                                InitialRequestedAddAmount);
		}
		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			// return partial result
			return FItemAddResult::AddedPartial(FText::Format(
				                                    FText::FromString(
					                                    "Partial amount of {0} added to the inventory. Number added = {1}"),
				                                    InputItem->TextData.Name, StackableAmountAdded),
			                                    StackableAmountAdded);
		}
		if (StackableAmountAdded <= 0)
		{
			// return none result
			return FItemAddResult::AddedNone(FText::Format(
				FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."),
				InputItem->TextData.Name));
		}
	}

	check(false);
	return FItemAddResult::AddedNone(FText::Format(
		FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."),
		InputItem->TextData.Name));
}

void UInventoryComponent::AddNewItem(UItemBase* OldItem, const int32 AmountToAdd)
{
	UItemBase* NewItem;

	// 만약 이미 아미템이 복사본이거나 집을 수 있다면
	if (OldItem->bIsCopy || OldItem->bIsPickup)
	{
		NewItem = OldItem;
		NewItem->ResetItemFlags();
	}
	// 그게 아니면
	else
	{
		NewItem = OldItem->CreateItemCopy();
	}
	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();
	OnInventoryUpdated.Broadcast();
}
