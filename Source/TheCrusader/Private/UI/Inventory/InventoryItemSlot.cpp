// Written by jaegang lim


#include "UI/Inventory/InventoryItemSlot.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/ItemDataStructs.h"
#include "Item/Data/ItemBase.h"
#include "UI/Inventory/InventoryActionMenu.h"
#include "UI/Inventory/InventoryTooltip.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (ToolTipClass)
	{
		UInventoryTooltip* Tooltip = CreateWidget<UInventoryTooltip>(this, ToolTipClass);
		Tooltip->InventorySlotBeingHovered = this;
		SetToolTip(Tooltip);
	}
}

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemReference)
	{
		switch (ItemReference->ItemData.ItemQuality)
		{
		case EItemQuality::Shoddy:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
		case EItemQuality::Common:
			ItemBorder->SetBrushColor(FLinearColor::White);
			break;
		case EItemQuality::Quality:
			ItemBorder->SetBrushColor(FLinearColor::Green);
			break;
		case EItemQuality::Masterwork:
			ItemBorder->SetBrushColor(FLinearColor::Blue);
			break;
		case EItemQuality::Grandmaster:
			ItemBorder->SetBrushColor(FLinearColor(100.f, 65.f, 0.f, 1.f)); // orange
			break;
		default: ;
		}
		ItemIcon->SetBrushFromTexture(ItemReference->ItemData.AssetData.Icon);

		if (ItemReference->ItemData.NumericData.bIsStackable)
		{
			ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Unhandled();

	if (!ItemReference)
		return Reply;

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		UInventoryActionMenu* ActionMenu = CreateWidget<UInventoryActionMenu>(this, ActionMenuClass);
		ActionMenu->AddToViewport(10);
		ActionMenu->SourceSlot = this;
		Reply = FReply::Handled();
	}
	return Reply;
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                              UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                      UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}
