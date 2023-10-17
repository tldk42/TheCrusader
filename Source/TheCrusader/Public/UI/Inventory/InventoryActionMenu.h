// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryActionMenu.generated.h"

class UTextBlock;
class UInventoryItemSlot;
class UVerticalBox;
class UButton;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UInventoryActionMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:
	UFUNCTION()
	virtual void Use();
	UFUNCTION()
	virtual void Drop();
	UFUNCTION()
	virtual void DropAll();

public:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* BoxActionMenu;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnUse;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnDrop;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnDropAll;

	UPROPERTY()
	UInventoryItemSlot* SourceSlot;

protected:
	
};
