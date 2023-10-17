// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class UInventoryItemSlot;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void RefreshInventory();

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	void SetInfoText() const;
	
public:

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* InventoryPanel;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WeightInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	// // 상점이거나 NPC가 소유할 수도 있음
	// UPROPERTY()
	// AActor* OwnerCharacter;

	UPROPERTY()
	UInventoryComponent* InventoryRef;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;
};
