// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetIndicator.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UTargetIndicator : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* IndicatorImage;
};
