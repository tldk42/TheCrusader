// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/SkillDataStruchts.h"
#include "Skill_Slot_Equipped.generated.h"

class USkill_Information;
class UImage;
class UBorder;

DECLARE_MULTICAST_DELEGATE(FOnSlotUpdated);


/**
 * 
 */
UCLASS()
class THECRUSADER_API USkill_Slot_Equipped : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnSlotUpdated OnSlotUpdated;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateSlot();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DeleteSlot();

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	ESkillKeyType SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSavedSkillUIData Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	bool bIsPassive;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* Border;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* Icon;
};
