// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Data/SkillDataStruchts.h"
#include "Skill_Slot.generated.h"

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
	USkill_Tree* GetSkillTreeWBP() const;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintNativeEvent)
	void TryToLearn();

	UFUNCTION(BlueprintNativeEvent)
	void Learn();
	
	UFUNCTION(BlueprintNativeEvent)
	void SetupSkillSlots();

private:
	void PlayVisualEffect();

	void SetStyles(const FSkill& SkillInfo) const;

	void UpdateBackgroundColor() const;

	TArray<USkill_Slot*> FindNextSlots();

	bool CanLearn() const;

	bool CheckRequiredSkills(const TArray<USkill_Slot*>& Skill_Slots,
	                         const TArray<FDataTableRowHandle>& RowHandles) const;

	static TArray<const USkill_Slot*> FindRequiredSkillWidgets(const TArray<USkill_Slot*>& Skill_Slots,
	                                                           const TArray<FDataTableRowHandle>& RowHandles);

	UFUNCTION()
	void OnHoveredEvent();

	UFUNCTION()
	void OnUnHoveredEvent();

	UFUNCTION()
	void OnClickedEvent();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SkillLevel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Logo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Info")
	FDataTableRowHandle Skill_DataTableRowHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Info")
	FSkill SkillInfo;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bool")
	bool bCanBeLearned;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bool")
	bool bLearned;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bool")
	bool bTryingToLearn;

	bool bHoveredTrigger = false; 
	bool bUnHoveredTrigger = false; 

#pragma endregion Flags

#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TArray<USkill_Slot*> AllSkillSlots;
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TArray<USkill_Slot*> NextSkillSlots;
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	UUserWidget* PaletteWidget;

#pragma endregion Widgets

#pragma region WidgetAnimation

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim))
	UWidgetAnimation* OnLearned;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim))
	UWidgetAnimation* OnUnHovered;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim))
	UWidgetAnimation* OnHovered;

#pragma endregion WidgetAnimation

#pragma region WidgetSound

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* DisAllowedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* AllowedSound;

#pragma endregion WidgetSound
};
