// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/SkillDataStruchts.h"
#include "Skill_Slot_Container.generated.h"

class USkill_Binding_Container;
/**
 * 
 */
UCLASS()
class THECRUSADER_API USkill_Slot_Container : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeContainer();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateSlot(ESkillKeyType KeyType, const UTexture2D* Texture, const bool Value);
};
