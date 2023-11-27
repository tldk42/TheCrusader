// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "NPCBarBase.h"
#include "EnemyBar.generated.h"

class UBorder;
class UProgressBar;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UEnemyBar : public UNPCBarBase
{
	GENERATED_BODY()

public:
	void UpdateHP(const float NewValue);
	void UpdateStamina(const float NewValue);

	void HighlightBorder() const;

	void UnHighlightBorder() const;

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY()
	bool bTargeted;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UImage* IconHighlightBorder;

	UPROPERTY(meta = (BindWidget))
	UBorder* HPBorder;
};
