// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemDataStructs.h"
#include "UObject/Object.h"
#include "ItemBase.generated.h"

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

	UFUNCTION(Category = "Item")
	UItemBase* CreateItemCopy() const;

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemStackWeight() const { return Quantity * NumericData.Weight; }

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemSingleWeight() const { return NumericData.Weight; }

	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == NumericData.MaxStackSize; }

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity);

	UFUNCTION(Category = "Item") 
	virtual void Use(ABalian* Character);

protected:
	bool operator==(const FName& OtherID) const
	{
		return this->ID == OtherID;
	}

public:
	// UPROPERTY()
	// UInventoryComponent* OwningInventory;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FName ID;
	
	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemQuality ItemQuality;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemStatistics ItemStatistics;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemTextData TextData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemNumericData NumericData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemAssetData AssetData;

};
