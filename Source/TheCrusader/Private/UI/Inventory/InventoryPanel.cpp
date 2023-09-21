// Written by jaegang lim


#include "UI/Inventory/InventoryPanel.h"

#include "Component/Inventory/InventoryComponent.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "UI/Inventory/InventoryItemSlot.h"

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                   UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryPanel::RefreshInventory()
{
	if (InventoryRef && InventorySlotClass)
	{
		// 패널UI의 모든것을 지움
		InventoryPanel->ClearChildren();

		// 패널에 아이템 하나씩 추가 (다시 만듦)
		for (UItemBase* const& InventoryItem : InventoryRef->GetInventoryContents())
		{
			UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
			ItemSlot->SetItemReference(InventoryItem);

			InventoryPanel->AddChildToWrapBox(ItemSlot);
		}

		SetInfoText();
	}
}

void UInventoryPanel::SetInfoText() const
{
	WeightInfo->SetText(FText::Format(FText::FromString("{0}/{1}"),
	                                  InventoryRef->GetInventoryTotalWeight(),
	                                  InventoryRef->GetWeightCapacity()));
	CapacityInfo->SetText(FText::Format(FText::FromString("{0}/{1}"),
	                                    InventoryRef->GetInventoryContents().Num(),
	                                    InventoryRef->GetSlotCapacity()));
}
