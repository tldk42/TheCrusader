// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Item/Data/ItemEquipmentBase.h"
#include "Item_Equipment.generated.h"

UCLASS()
class THECRUSADER_API AItem_Equipment : public AItem
{
	GENERATED_BODY()

public:
	AItem_Equipment();

	FORCEINLINE virtual UItemEquipmentBase* GetItemData() const override
	{
		return Cast<UItemEquipmentBase>(ItemRef);
	}

protected:
	virtual void BeginPlay() override;

public:
	bool bEquipped;
	
};
