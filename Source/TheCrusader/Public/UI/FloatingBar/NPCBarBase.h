// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCBarBase.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UNPCBarBase : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(meta = (BindWidget))
	UImage* IconImage;
};
