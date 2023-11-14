// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInGameUI.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UPlayerInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Player Health",
		meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	UMaterialInstanceDynamic* HealthMaterial;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Player Stamina",
		meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	UMaterialInstanceDynamic* StaminaMaterial;
};
