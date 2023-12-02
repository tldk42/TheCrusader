// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Skill_Information.generated.h"

class USkill_Slot;
class UTextBlock;
/**
 * 
 */
UCLASS()
class THECRUSADER_API USkill_Information : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateToolTip();

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkill_Slot* SkillSlotBeingHovered;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SkillName_Text;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SkillDescription_Text;
};
