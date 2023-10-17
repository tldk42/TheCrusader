// Written by jaegang lim


#include "UI/Inventory/EquippedActionMenu.h"

#include "Character/Balian.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Item/Data/ItemBase.h"
#include "UI/Inventory/InventoryItemSlot.h"

void UEquippedActionMenu::NativeConstruct()
{
	Super::NativeConstruct();

	BtnDrop->SetVisibility(ESlateVisibility::Collapsed);
	BtnDropAll->SetVisibility(ESlateVisibility::Collapsed);
}

void UEquippedActionMenu::Use()
{
	// 장착해제
	SourceSlot->ItemReference->UnEquip(Cast<ABalian>(GetOwningPlayerPawn()));
}
