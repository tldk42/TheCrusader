// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageText.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UDamageText : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	float DamageAmount;

	UPROPERTY(BlueprintReadOnly)
	FVector2D WidgetPos;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DamageText;
};
