// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Interfaces/HealthHUD.h"
#include "TC_HUD.generated.h"

enum class EWeaponType : uint8;
class UPlayerInGameUI;
class URadialMenu;
struct FInteractableData;
class UInteractionWidget;
class UMainMenu;
/**
 * 
 */
UCLASS()
class THECRUSADER_API ATC_HUD : public AHUD, public IHealthHUD
{
	GENERATED_BODY()

public:
	ATC_HUD();

public:
	void DisplayMenu();
	void ToggleMenu();
	void HideMenu();

	void ShowRadialWidget();
	void HideRadialWidget();
	void UpdateActiveRadialWidget(const int Index) const;

	UFUNCTION(BlueprintCallable)
	void ShowCrosshair() const;
	UFUNCTION(BlueprintCallable)
	void HideCrosshair() const;

	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;

	FORCEINLINE UUserWidget* GetCrosshairWidget() const
	{
		return CrosshairWidget;
	};

	virtual void SetHP(float InPercent) override;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenu> MainMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UPlayerInGameUI> PlayerInGameUIClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CrosshairClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	bool bIsMenuVisible;

	bool bIsRadialMenuVisible;

protected:
	UPROPERTY()
	UMainMenu* MainMenuWidget;

	UPROPERTY(BlueprintReadWrite)
	URadialMenu* RadialMenuWidget;

	UPROPERTY()
	UPlayerInGameUI* PlayerInGameUI;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* CrosshairWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;
};
