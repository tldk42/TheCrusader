﻿// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTooltip.generated.h"

class UTextBlock;
class UInventoryItemSlot;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UInventoryTooltip : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateToolTip();
	
protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(VisibleAnywhere)
	UInventoryItemSlot* InventorySlotBeingHovered;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemType;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageValue;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ArmorRating;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* UsageText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxStackSize;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SellValue;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StackWeight;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Power;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Armour;
};
