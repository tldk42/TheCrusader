// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/InventoryPanel.h"
#include "PlayerInventoryPanel.generated.h"

class UItemBase;
enum class EEquipmentPart : uint8;
class UPlayerItemSlot;
class ABalian;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UPlayerInventoryPanel : public UInventoryPanel
{
	GENERATED_BODY()

public:
	virtual void RefreshInventory() override;

protected:
	virtual void NativeOnInitialized() override;

public:
	UPROPERTY()
	ABalian* PlayerRef;

protected:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UPlayerItemSlot* HeadSlot;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UPlayerItemSlot* TorsoSlot;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UPlayerItemSlot* ArmSlot;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UPlayerItemSlot* PantsSlot;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UPlayerItemSlot* FeetSlot;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UPlayerItemSlot* WeaponSlot;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UPlayerItemSlot* ShieldSlot;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UPlayerItemSlot* BowSlot;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* PowerText;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* ArmourText;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* SkillText;
};
