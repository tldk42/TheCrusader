// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/Radial/RadialButtonBase.h"
#include "TCPlayerController.generated.h"

class ATC_HUD;
class ABalian;
/**
 * 
 */
UCLASS()
class THECRUSADER_API ATCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	bool UpdatePlayerState(EButtonType Type);
	void ToggleMenu();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void ShowRadialMenu();
	void CloseRadialMenu();

private:
	UPROPERTY()
	ATC_HUD* HUD;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputMappingContext* ControllerMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* RadialMenuAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	UInputAction* ToggleMenuAction;

	UPROPERTY(BlueprintReadWrite, Category = "Reference", meta = (AllowPrivateAccess = true))
	ABalian* PlayerRef;
};
