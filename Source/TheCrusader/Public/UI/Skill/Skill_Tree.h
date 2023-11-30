// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Skill_Tree.generated.h"

class USkill_Slot;
struct FDataTableRowHandle;
/**
 * 
 */
UCLASS()
class THECRUSADER_API USkill_Tree : public UUserWidget
{
	GENERATED_BODY()

public:
	void SkillSlotOnHovered(const FDataTableRowHandle& DataTableRowHandle, const USkill_Slot* Skill_Slot, bool bLearned, bool bCanBeLearned, bool bEnoughPoint);
	void SkillSlotOnUnHovered();
	void UpdateSkillsResult();
	TArray<USkill_Slot*> GetSkill_Slots(UUserWidget* ParentWidget);
};
