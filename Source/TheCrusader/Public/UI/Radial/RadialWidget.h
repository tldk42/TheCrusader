// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnItemSelected);

/**
 * 
 */
UCLASS()
class THECRUSADER_API URadialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnItemSelected ItemSelected;

protected:
	// virtual void NativePreConstruct() override;
	// virtual void NativeConstruct() override;
	// virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
};
