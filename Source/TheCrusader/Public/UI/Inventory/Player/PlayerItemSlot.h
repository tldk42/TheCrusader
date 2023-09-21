// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Inventory/InventoryItemSlot.h"
#include "PlayerItemSlot.generated.h"

enum class EEquipmentPart : uint8;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UPlayerItemSlot : public UInventoryItemSlot
{
	GENERATED_BODY()

public:
	void UpdateItemSlot(UItemBase* Item);
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Slot", meta = (BindWidget))
	UTextBlock* SlotNameText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Slot")
	EEquipmentPart SlotName;
};
