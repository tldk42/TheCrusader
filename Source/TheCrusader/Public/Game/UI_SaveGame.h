// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Data/SkillDataStruchts.h"
#include "GameFramework/SaveGame.h"
#include "UI_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API UUI_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<ESkillKeyType, FSavedSkillUIData> SavedUIData;
};
