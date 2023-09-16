// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/InventoryPanel.h"
#include "PlayerInventoryPanel.generated.h"

class ABalian;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UPlayerInventoryPanel : public UInventoryPanel
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	
public:
	UPROPERTY()
	ABalian* PlayerRef;
};
