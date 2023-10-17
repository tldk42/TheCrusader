// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemPickup.h"
#include "Item/Data/ItemEquipmentBase.h"
#include "Item_Equipment.generated.h"

UCLASS()
class THECRUSADER_API AItem_Equipment : public AItemPickup
{
	GENERATED_BODY()

public:
	AItem_Equipment();

	FORCEINLINE virtual UItemEquipmentBase* GetItemData() const override
	{
		return Cast<UItemEquipmentBase>(ItemRef);
	}

	virtual void InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity) override;

protected:
	virtual void BeginPlay() override;

public:
	bool bEquipped;
	
};
