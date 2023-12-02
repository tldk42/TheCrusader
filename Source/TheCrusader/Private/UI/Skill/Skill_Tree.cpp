// Written by jaegang lim


#include "UI/Skill/Skill_Tree.h"

#include "Component/Skill/SkillComponent.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Skill/Skill_Category.h"
#include "UI/Skill/Skill_Information.h"
#include "UI/Skill/Skill_Line.h"
#include "UI/Skill/Skill_TotalSummary.h"


void USkill_Tree::NativeConstruct()
{
	Super::NativeConstruct();

	InformationWidgetBorder.MinX = -688.f;
	InformationWidgetBorder.MaxX = 688.f;
	InformationWidgetBorder.MinY = -176.f;
	InformationWidgetBorder.MaxY = -355.f;

	UpdateSkillsResult();

	LoadAllSkills(WidgetSwitcher->GetAllChildren());
}

void USkill_Tree::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void USkill_Tree::OnRemoveFromParent() const
{
}

void USkill_Tree::LoadSkills(UWidget* SkillCategory) const
{
	USkill_Category* SC = Cast<USkill_Category>(SkillCategory);
	if (!SkillCategory)
		return;

	USkillComponent* SkillComponent = Cast<USkillComponent>(
		GetOwningPlayerPawn()->GetComponentByClass(USkillComponent::StaticClass()));
	if (SkillComponent)
	{
		if (SkillComponent->LoadSkills())
		{
			for (const FPerksList& Perks : SkillComponent->GetPerksList())
			{
				for (USkill_Slot* SS : SC->GetSkillSlots())
				{
					if (Perks.bLearned)
					{
						if (SS->GetSkillName() == Perks.RowHandle.RowName)
						{
							SS->LearnSaves();
							break;
						}
					}
				}
			}
		}
	}
}

void USkill_Tree::LoadAllSkills(TArray<UWidget*> SkillCategory) const
{
	TArray<USkill_Category*> SC;

	for (const auto& Category : SkillCategory)
	{
		if (USkill_Category* CastedCategory = Cast<USkill_Category>(Category))
		{
			SC.Add(CastedCategory);
		}
	}
	if (SC.Num() <= 0)
		return;

	USkillComponent* SkillComponent = Cast<USkillComponent>(
		GetOwningPlayerPawn()->GetComponentByClass(USkillComponent::StaticClass()));
	if (SkillComponent)
	{
		if (SkillComponent->LoadSkills())
		{
			for (const USkill_Category* Category : SC)
			{
				for (const FPerksList& Perks : SkillComponent->GetPerksList())
				{
					TArray<USkill_Slot*> Skill_Slots = Category->GetSkillSlots();
					if (Skill_Slots.Num() > 0)
					{
						for (USkill_Slot* SS : Skill_Slots)
						{
							if (Perks.bLearned)
							{
								if (SS->GetSkillName() == Perks.RowHandle.RowName)
								{
									SS->LearnSaves();
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

void USkill_Tree::SkillSlotOnHovered(const FDataTableRowHandle& DataTableRowHandle, USkill_Slot* Skill_Slot,
                                     const bool bLearned, const bool bCanBeLearned, const bool bEnoughPoint) const
{

}

void USkill_Tree::SkillSlotOnUnHovered() const
{

}

void USkill_Tree::UpdateSkillsResult() const
{
	const USkill_Category* Skill_Category = Cast<USkill_Category>(WidgetSwitcher->GetAllChildren()[CurrentMenuIndex]);
	if (Skill_Category)
	{
		int LearnedPerks;
		int AllPerks;
		int LearnedPerksPoints;
		int AllPerksPoint;
		FName Name;
		Skill_Category->GetPerks(LearnedPerks, AllPerks, LearnedPerksPoints, AllPerksPoint, Name);
		// Update Info
		if (Skill_Summary)
		{
			Skill_Summary->InitializeSummary(FText::FromName(Name), AllPerks, LearnedPerks);
		}

	}
}

TArray<USkill_Slot*> USkill_Tree::GetSkill_Slots(const USkill_Category* ParentWidget)
{
	if (!ParentWidget)
	{
		return TArray<USkill_Slot*>();
	}
	return ParentWidget->GetSkillSlots();
}

TArray<USkill_Line*> USkill_Tree::GetConnectedLines(USkill_Category* Parent) const
{
	if (IsValid(Parent))
	{
		const USkill_Category* Skill_Category = Cast<USkill_Category>(
			WidgetSwitcher->GetWidgetAtIndex(CurrentMenuIndex));
		if (Skill_Category)
		{
			return Skill_Category->GetLines();
		}
	}
	return TArray<USkill_Line*>();
}
