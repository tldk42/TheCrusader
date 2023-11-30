// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Data/SkillDataStruchts.h"
#include "GameFramework/SaveGame.h"
#include "Skill_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API USkill_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FSavedSkill SavedSkills;
};
