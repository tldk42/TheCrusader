// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "ItemEquipmentBase.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API UItemEquipmentBase : public UItemBase
{
	GENERATED_BODY()

protected:
	virtual UItemBase* CreateItemCopy() const override;
	virtual AItem* Drop(int32 NumToRemove) override;
	virtual void Use(ABalian* Character) override;
	virtual void UnEquip(ABalian* Character) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	EEquipmentPart EquipmentPart;
	
	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	FEquipmentData EquipmentData;
};
