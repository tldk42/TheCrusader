// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemPickup.h"
#include "Item/Data/ItemWeaponBase.h"
#include "MyItem.generated.h"

UCLASS()
class THECRUSADER_API AMyItem : public AItemPickup
{
	GENERATED_BODY()

public:
	AMyItem();

	FORCEINLINE virtual UItemWeaponBase* GetItemData() const override
	{
		return Cast<UItemWeaponBase>(ItemRef);
	}

	virtual void InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity) override;

protected:
	virtual void BeginPlay() override;

public:
	bool bEquipped;
};
