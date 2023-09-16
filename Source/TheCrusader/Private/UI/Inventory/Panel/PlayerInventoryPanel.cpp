// Written by jaegang lim


#include "UI/Inventory/Panel/PlayerInventoryPanel.h"

#include "Character/Balian.h"
#include "Component/Inventory/InventoryComponent.h"

void UPlayerInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerRef = Cast<ABalian>(GetOwningPlayerPawn());

	if (PlayerRef)
	{
		InventoryRef = PlayerRef->GetInventory();
		if (InventoryRef)
		{
			InventoryRef->OnInventoryUpdated.AddUObject(this, &ThisClass::RefreshInventory);
			SetInfoText();
		}
	}
}
