// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Data/SkillDataStruchts.h"
#include "Skill_Slot.generated.h"

class USkill_Information;
class USkill_Line;
class USkill_Category;
class USkill_Tree;
class UButton;
class UProgressBar;
/**
 * 
 */
UCLASS()
class THECRUSADER_API USkill_Slot : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void LearnSaves();

	USkill_Tree* GetSkillTreeWBP() const;

	FORCEINLINE FName GetSkillName() const { return Skill_DataTableRowHandle.RowName; }

	void SetCategoryWidget(USkill_Category* InCategory);

	bool CheckRequiredSkills(const TArray<USkill_Slot*>& Skill_Slots,
	                         const TArray<FDataTableRowHandle>& RowHandles) const;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintNativeEvent)
	void TryToLearn();

	UFUNCTION(BlueprintNativeEvent)
	void Learn();


	UFUNCTION(BlueprintCallable)
	void SetupSkillSlotsOfCurrentCategory();

private:
	void PlayVisualEffect();

	void SetStyles(const FSkill& InSkillInfo) const;

	void SetAdjacentSlots();

	void UpdateBackgroundColor() const;

	TArray<USkill_Slot*> FindNextSlots();

	void FindConnectors();

	bool CanLearn() const;

	static TArray<const USkill_Slot*> FindRequiredSkillWidgets(const TArray<USkill_Slot*>& Skill_Slots,
	                                                           const TArray<FDataTableRowHandle>& RowHandles);

	UFUNCTION()
	void OnHoveredEvent();

	UFUNCTION()
	void OnUnHoveredEvent();

	UFUNCTION()
	void OnClickedEvent();

protected:
#pragma region Data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Info", meta = (ExposeOnSpawn))
	FDataTableRowHandle Skill_DataTableRowHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Info")
	FSkill SkillInfo;
#pragma endregion Data

#pragma region Colors
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Color")
	FLinearColor CannotBeLearnedColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Color")
	FLinearColor CanBeLearnedColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Color")
	FLinearColor LearnedColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Color")
	FLinearColor TryingToLearningColor;
#pragma endregion Colors

#pragma region Flags

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bool", meta = (ExposeOnSpawn))
	bool bCanBeLearned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bool", meta = (ExposeOnSpawn))
	bool bLearned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bool", meta = (ExposeOnSpawn))
	bool bTryingToLearn;

	bool bTimerState;

	bool bHoveredTrigger = false;
	bool bUnHoveredTrigger = false;

#pragma endregion Flags

#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SkillLevel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Logo;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TArray<USkill_Slot*> AllSkillSlots;
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TArray<USkill_Slot*> NextSkillSlots;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TArray<USkill_Line*> NextLines;
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TArray<USkill_Line*> PreviousLines;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	USkill_Category* CategoryWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<USkill_Information> SkillToolTipClass;
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	USkill_Information* SkillToolTip;

#pragma endregion Widgets

#pragma region WidgetAnimation

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim ), Transient)
	UWidgetAnimation* OnLearned;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim ), Transient)
	UWidgetAnimation* OnUnHovered;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim ), Transient)
	UWidgetAnimation* OnHovered;

#pragma endregion WidgetAnimation

#pragma region WidgetSound

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* DisAllowedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* AllowedSound;

#pragma endregion WidgetSound
};
