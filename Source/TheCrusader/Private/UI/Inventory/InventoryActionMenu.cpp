// Written by jaegang lim


#include "UI/Inventory/InventoryActionMenu.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/Balian.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Item/Data/ItemBase.h"
#include "UI/Inventory/InventoryItemSlot.h"

void UInventoryActionMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UWidgetLayoutLibrary::SlotAsCanvasSlot(BoxActionMenu)->SetPosition(
		UWidgetLayoutLibrary::GetMousePositionOnViewport(this));

	BtnUse->OnClicked.AddDynamic(this, &ThisClass::Use);
	BtnDrop->OnClicked.AddDynamic(this, &ThisClass::Drop);
	BtnDropAll->OnClicked.AddDynamic(this, &ThisClass::DropAll);
}

void UInventoryActionMenu::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	RemoveFromParent();
}

void UInventoryActionMenu::Use()
{
	if (UItemBase* Item = SourceSlot->ItemReference)
	{
		if (ABalian* Player = Cast<ABalian>(GetOwningPlayerPawn()))
		{
			Item->Use(Player);
		}
	}

	RemoveFromParent();
}

void UInventoryActionMenu::Drop()
{
	if (UItemBase* Item = SourceSlot->ItemReference)
	{
		Item->Drop(1);
	}
	RemoveFromParent();
}

void UInventoryActionMenu::DropAll()
{
	if (UItemBase* Item = SourceSlot->ItemReference)
	{
		Item->Drop(Item->Quantity);
	}
	RemoveFromParent();
}
