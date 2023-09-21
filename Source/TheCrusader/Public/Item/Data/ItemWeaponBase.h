// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "ItemWeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API UItemWeaponBase : public UItemBase
{
	GENERATED_BODY()

protected:
	virtual UItemBase* CreateItemCopy() const override;
	virtual void Use(ABalian* Character) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	FWeaponData WeaponData;
	
};
