// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemDataStructs.h"
#include "UObject/Object.h"
#include "ItemBase.generated.h"

class AItem;
class UInventoryComponent;
class ABalian;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UItemBase : public UObject
{
	GENERATED_BODY()

public:
	UItemBase();

	void ResetItemFlags();

	UFUNCTION(Category = "Item")
	virtual UItemBase* CreateItemCopy() const;

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemStackWeight() const { return Quantity * ItemData.NumericData.Weight; }

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemSingleWeight() const { return ItemData.NumericData.Weight; }

	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == ItemData.NumericData.MaxStackSize; }

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity);

	UFUNCTION(Category = "Item")
	virtual void Use(ABalian* Character);

	UFUNCTION(Category = "Item")
	virtual AItem* Drop(int32 NumToRemove);

	virtual void UnEquip(ABalian* Character);

protected:
	bool operator==(const FName& OtherID) const
	{
		return this->ItemData.ItemID == OtherID;
	}

public:
	UPROPERTY()
	UInventoryComponent* OwningInventory;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FInventoryItem ItemData;

	bool bIsCopy;
	bool bIsPickup;
};
