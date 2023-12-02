// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/SkillDataStruchts.h"
#include "Skill_Tree.generated.h"

class USkill_Information;
class USkill_TotalSummary;
class USkill_Line;
class USkill_Category;
class UWidgetSwitcher;
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
	static TArray<USkill_Slot*> GetSkill_Slots(const USkill_Category* ParentWidget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void GetPerks(int& LearnedPerks, int& AllPerks, int& LearnedPerksPoint, int& AllPoints, FName& Name) const;

	UFUNCTION(BlueprintCallable)
	TArray<USkill_Line*> GetConnectedLines(USkill_Category* Parent) const;

	UFUNCTION(BlueprintCallable)
	void UpdateSkillsResult() const;

	void SkillSlotOnHovered(const FDataTableRowHandle& DataTableRowHandle, USkill_Slot* Skill_Slot, bool bLearned,
	                        bool bCanBeLearned, bool bEnoughPoint) const;
	void SkillSlotOnUnHovered() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	void OnRemoveFromParent() const;

	void LoadSkills(UWidget* SkillCategory) const;

	void LoadAllSkills(TArray<UWidget*> SkillCategory) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FSkillInformationWidgetBorder InformationWidgetBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USkill_TotalSummary* Skill_Summary;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int CurrentMenuIndex;
};
