// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "InventoryActionMenu.h"
#include "EquippedActionMenu.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API UEquippedActionMenu : public UInventoryActionMenu
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void Use() override;
};
