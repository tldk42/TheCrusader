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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Health", meta = (BindWidget))
	UProgressBar* HealthBar;
};
