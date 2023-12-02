// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Skill_Category.generated.h"

class USkill_Line;
class USkill_Slot;
/**
 * 
 */
UCLASS()
class THECRUSADER_API USkill_Category : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	TArray<USkill_Slot*> GetSkillSlots() const;

	TArray<USkill_Line*> GetLines() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void GetPerks(int& LearnedPerks, int& AllPerks, int& LearnedPerksPoint, int& AllPoints, FName& Name) const;

	void SetupSkillSlots(const UPanelWidget* Target);

protected:
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* CategoryPanel;

	UPROPERTY(BlueprintReadOnly)
	TArray<USkill_Slot*> Skill_Slots;
	UPROPERTY(BlueprintReadOnly)
	TArray<USkill_Line*> Skill_Lines;
};
