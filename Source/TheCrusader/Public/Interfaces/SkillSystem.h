// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Data/SkillDataStruchts.h"
#include "UObject/Interface.h"
#include "SkillSystem.generated.h"

class USkill_Slot;

UINTERFACE()
class USkillSystem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THECRUSADER_API ISkillSystem
{
	GENERATED_BODY()

public:
	// virtual void OnMaxValueUpdated(ESkillType SkillType);
	//
	// virtual TArray<USkill_Slot*> GetSkillSlots(UUserWidget* Parent);

	
};
