// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Skill_TotalSummary.generated.h"

class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class THECRUSADER_API USkill_TotalSummary : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSummary(const FText& Title, int AllPointToLearn, int LearnedPoint);

	void SetTitleText(const FText& NewText);
	void SetLearnedPoints_Num(const int NewValue);

protected:

private:
	void UpdateProgressbar() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Progress_Text;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CategoryTitle_Text;

private:
	UPROPERTY(VisibleAnywhere)
	FText CategoryTitle;
	UPROPERTY(VisibleAnywhere)
	int TotalLearn_Num;
	UPROPERTY(VisibleAnywhere)
	int Learned_Num;
};
