// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Skill_Binding_Container.generated.h"

class UHorizontalBox;
/**
 * 
 */
UCLASS()
class THECRUSADER_API USkill_Binding_Container : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SaveData() const;
	UFUNCTION(BlueprintCallable)
	void LoadData() const;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* PassiveSkills;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* ActiveSkills;
};
