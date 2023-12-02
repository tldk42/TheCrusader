// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Skill_Slot.h"
#include "Blueprint/UserWidget.h"
#include "Skill_Line.generated.h"

class USkill_Category;
class USkill_Slot;
class UImage;
/**
 * 
 */
UCLASS()
class THECRUSADER_API USkill_Line : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE FName GetPrevSkillName() const { return PreviousSkill_Name; }
	FORCEINLINE FName GetNextSkillName() const { return NextSkill_Name; }

	void SetNextCanBeLearned(bool bNew);
	void SetPreviousCanBeLearned(bool bNew);
	void SetCategoryWidget(USkill_Category* InCategory);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ControlColours();

protected:
	virtual void NativePreConstruct() override;

	UFUNCTION(BlueprintNativeEvent)
	void Setup();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetColours(const FLinearColor& Color1, const FLinearColor& Color2);


	UFUNCTION(BlueprintCallable)
	void FindCorrectSlots(UPARAM(ref)USkill_Slot*& PreviousSlot, UPARAM(ref)USkill_Slot*& NextSlot);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* LineImage;

	UPROPERTY(BlueprintReadWrite)
	USkill_Category* Skill_Category;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (ExposeOnSpawn))
	FName PreviousSkill_Name;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (ExposeOnSpawn))
	FName NextSkill_Name;

#pragma region Colour

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colour")
	FLinearColor CanBeLearnedColour;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colour")
	FLinearColor CannotBeLearnedColour;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colour")
	FLinearColor LearnedColour;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colour")
	FLinearColor CustomColour2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colour")
	FLinearColor CustomColour1;

#pragma endregion Colour

#pragma region Flag

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bool", meta = (ExposeOnSpawn))
	bool bVertical;

	UPROPERTY(BlueprintReadOnly, Category = "Bool")
	bool bPreviousLearned;

	UPROPERTY(BlueprintReadOnly, Category = "Bool")
	bool bPreviousCanBeLearned;

	UPROPERTY(BlueprintReadOnly, Category = "Bool")
	bool bNextLearned;

	UPROPERTY(BlueprintReadOnly, Category = "Bool")
	bool bNextCanBeLearned;
#pragma endregion Flag
};
