// Written by jaegang lim


#include "UI/Skill/Skill_Line.h"

#include "Components/Image.h"
#include "UI/Skill/Skill_Category.h"

void USkill_Line::SetCategoryWidget(USkill_Category* InCategory)
{
	Skill_Category = InCategory;
}

void USkill_Line::SetNextCanBeLearned(bool bNew)
{
	bNextCanBeLearned = bNew;
}

void USkill_Line::SetPreviousCanBeLearned(const bool bNew)
{
	bPreviousCanBeLearned = bNew;
}

void USkill_Line::NativePreConstruct()
{
	Super::NativePreConstruct();

	Setup();

	ControlColours();
}

void USkill_Line::Setup_Implementation()
{
	if (bVertical)
	{
		LineImage->SetRenderTransformAngle(90.f);
	}
}

void USkill_Line::SetColours_Implementation(const FLinearColor& Color1, const FLinearColor& Color2)
{
	// 색상 지정
}

void USkill_Line::FindCorrectSlots(UPARAM(ref)USkill_Slot*& PreviousSlot,UPARAM(ref) USkill_Slot*& NextSlot)
{
	if (IsValid(Skill_Category))
	{
		TArray<USkill_Slot*> Skill_Slots = Skill_Category->GetSkillSlots();
		if (Skill_Slots.Num() > 0)
		{
			for (USkill_Slot* Skill_Slot : Skill_Slots)
			{
				if (Skill_Slot->GetSkillName() == PreviousSkill_Name)
				{
					PreviousSlot = Skill_Slot;
				}
				else if (Skill_Slot->GetSkillName() == NextSkill_Name)
				{
					NextSlot = Skill_Slot;
				}
			}
		}
	}
}
