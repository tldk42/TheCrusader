// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerPanel.generated.h"

class UPlayerItemSlot;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UPlayerPanel : public UUserWidget
{
	GENERATED_BODY()

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
	UPlayerItemSlot* BowSlot;
	
};
