// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryActionMenu.generated.h"

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

private:
	void SpawnItem(int32 Quantity) const;
	
	UFUNCTION()
	void Use();
	UFUNCTION()
	void Drop();
	UFUNCTION()
	void DropAll();

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
};
