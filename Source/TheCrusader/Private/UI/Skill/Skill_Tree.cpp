// Written by jaegang lim


#include "UI/Skill/Skill_Tree.h"

void USkill_Tree::SkillSlotOnHovered(const FDataTableRowHandle& DataTableRowHandle, const USkill_Slot* Skill_Slot,
                                     bool bLearned, bool bCanBeLearned, bool bEnoughPoint)
{
}

void USkill_Tree::SkillSlotOnUnHovered()
{
}

void USkill_Tree::UpdateSkillsResult()
{
}

TArray<USkill_Slot*> USkill_Tree::GetSkill_Slots(UUserWidget* ParentWidget)
{
	TArray<USkill_Slot*> NewSlots;
	return NewSlots;
}
