// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TC_HUD.generated.h"

class URadialMenu;
struct FInteractableData;
class UInteractionWidget;
class UMainMenu;
/**
 * 
 */
UCLASS()
class THECRUSADER_API ATC_HUD : public AHUD
{
	GENERATED_BODY()

public:
	ATC_HUD();

public:
	void DisplayMenu();
	void ToggleMenu();
	void HideMenu();

	void ShowRadialWidget() const;
	void HideRadialWidget() const;

	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenu> MainMenuClass;

	// UPROPERTY(EditInstanceOnly, Category = "Widgets")
	// TSubclassOf<URadialMenu> RadialMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	bool bIsMenuVisible;

protected:
	UPROPERTY()
	UMainMenu* MainMenuWidget;

	UPROPERTY(BlueprintReadWrite)
	URadialMenu* RadialMenuWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;
};
