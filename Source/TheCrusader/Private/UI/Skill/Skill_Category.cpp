// Written by jaegang lim


#include "UI/Skill/Skill_Category.h"

#include "Components/PanelWidget.h"
#include "UI/Skill/Skill_Line.h"
#include "UI/Skill/Skill_Slot.h"

void USkill_Category::NativeConstruct()
{
	Super::NativeConstruct();

	Skill_Slots.Empty();
	Skill_Lines.Empty();

	SetupSkillSlots(CategoryPanel);
}

TArray<USkill_Line*> USkill_Category::GetLines() const
{
	return Skill_Lines;
}

void USkill_Category::SetupSkillSlots(const UPanelWidget* Target)
{
	for (UWidget* Element : Target->GetAllChildren())
	{
		if (USkill_Slot* Skill_Slot = Cast<USkill_Slot>(Element))
		{
			Skill_Slots.AddUnique(Skill_Slot);
			Skill_Slot->SetCategoryWidget(this);
		}
		else if (USkill_Line* Skill_Line = Cast<USkill_Line>(Element))
		{
			Skill_Lines.AddUnique(Skill_Line);
			Skill_Line->SetCategoryWidget(this);
		}
	}
}

TArray<USkill_Slot*> USkill_Category::GetSkillSlots() const
{
	return Skill_Slots;
}
