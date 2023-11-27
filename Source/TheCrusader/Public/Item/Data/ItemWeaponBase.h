// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "ItemEquipmentBase.h"
#include "ItemWeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API UItemWeaponBase : public UItemEquipmentBase
{
	GENERATED_BODY()
public:
	virtual void MoveToEquipment(ABalian* Character) override;
	
protected:
	virtual AItem* Drop(int32 NumToRemove) override;
	virtual void Use(ABalian* Character) override;
	virtual void UnEquip(ABalian* Character) override;
};
