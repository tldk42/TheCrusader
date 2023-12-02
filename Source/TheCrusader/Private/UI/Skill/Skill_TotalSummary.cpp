// Written by jaegang lim


#include "UI/Skill/Skill_TotalSummary.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USkill_TotalSummary::InitializeSummary(const FText& Title, const int AllPointToLearn, const int LearnedPoint)
{
	TotalLearn_Num = AllPointToLearn;
	SetTitleText(Title);
	SetLearnedPoints_Num(LearnedPoint);
	UpdateProgressbar();
}

void USkill_TotalSummary::SetTitleText(const FText& NewText)
{
	CategoryTitle = NewText;

	CategoryTitle_Text->SetText(CategoryTitle);
}

void USkill_TotalSummary::SetLearnedPoints_Num(const int NewValue)
{
	Learned_Num = NewValue;
	UpdateProgressbar();
}

void USkill_TotalSummary::UpdateProgressbar() const
{
	const float Progress = Learned_Num / TotalLearn_Num;
	ProgressBar->SetPercent(Progress);

	Progress_Text->SetText(FText::AsPercent(Progress));
}
