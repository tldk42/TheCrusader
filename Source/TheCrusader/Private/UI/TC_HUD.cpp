// Written by jaegang lim


#include "UI/TC_HUD.h"

#include "Character/Balian.h"
#include "Components/ProgressBar.h"
#include "Game/TheCrusaderGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenu.h"
#include "UI/Interaction/InteractionWidget.h"
#include "UI/PlayerWindow/PlayerInGameUI.h"
#include "UI/Radial/RadialMenu.h"

ATC_HUD::ATC_HUD()
{
}

void ATC_HUD::BeginPlay()
{
	Super::BeginPlay();

	const ATheCrusaderGameMode* GameMode = Cast<ATheCrusaderGameMode>(GetWorld()->GetAuthGameMode());

	PlayerPawn = Cast<ABalian>(GameMode->PlayerCharacterRef);


	// CPP에서 위젯을 생성할때는 Static클래스를 이용할 수 없다. CPP side의 위젯은 그래픽관련된게 없음
	if (MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuClass);
		MainMenuWidget->AddToViewport(5);
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	// if (RadialMenuClass)
	// {
	// 	RadialMenuWidget = CreateWidget<URadialMenu>(GetWorld(), RadialMenuClass);
	// 	RadialMenuWidget->AddToViewport();
	// 	RadialMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	// }

	if (CrosshairClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairClass);
		CrosshairWidget->AddToViewport();
		HideCrosshair();
	}

	if (PlayerInGameUIClass)
	{
		PlayerInGameUI = CreateWidget<UPlayerInGameUI>(GetWorld(), PlayerInGameUIClass);
		PlayerInGameUI->AddToViewport(-1);
		PlayerInGameUI->SetVisibility(ESlateVisibility::Visible);
	}


	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ATC_HUD::DisplayMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuVisible = true;
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ATC_HUD::ToggleMenu()
{
	if (PlayerPawn)
	{
		// Player->ActivateInventoryCamera(!bIsMenuVisible);

		if (bIsMenuVisible)
		{
			HideMenu();
			UGameplayStatics::SetGamePaused(GetWorld(), false);
			const FInputModeGameOnly InputModeGameOnly;
			GetOwningPlayerController()->SetInputMode(InputModeGameOnly);
			GetOwningPlayerController()->SetShowMouseCursor(false);
		}
		else
		{
			DisplayMenu();
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			const FInputModeGameAndUI InputModeUI;
			GetOwningPlayerController()->SetInputMode(InputModeUI);
			GetOwningPlayerController()->SetShowMouseCursor(true);
		}
	}
}

void ATC_HUD::HideMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuVisible = false;
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ATC_HUD::ShowRadialWidget()
{
	if (RadialMenuWidget)
	{
		RadialMenuWidget->SetVisibility(ESlateVisibility::Visible);
		bIsRadialMenuVisible = true;
	}
}

void ATC_HUD::HideRadialWidget()
{
	if (RadialMenuWidget)
	{
		RadialMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		RadialMenuWidget->UpdateActiveBar();
		bIsRadialMenuVisible = false;
	}
}

void ATC_HUD::UpdateActiveRadialWidget(const int Index) const
{
	if (RadialMenuWidget)
	{
		RadialMenuWidget->UpdateActiveBar(Index);
	}
}

void ATC_HUD::ShowCrosshair() const
{
	if (CrosshairWidget)
	{
		CrosshairWidget->SetIsEnabled(true);
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ATC_HUD::HideCrosshair() const
{
	if (CrosshairWidget)
	{
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
		CrosshairWidget->SetIsEnabled(false);
	}
}

void ATC_HUD::ShowInteractionWidget() const
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ATC_HUD::HideInteractionWidget() const
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ATC_HUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (InteractionWidget)
	{
		InteractionWidget->UpdateWidget(InteractableData);

		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ATC_HUD::SetHP(const float InPercent) const
{
	PlayerInGameUI->HealthMaterial->SetScalarParameterValue(FName("Percentage"), InPercent);
}

void ATC_HUD::SetStamina(const float InPercent) const
{
	PlayerInGameUI->StaminaMaterial->SetScalarParameterValue(FName("Percentage"), InPercent);
}

UMainMenu* ATC_HUD::GetMainMenuWidget() const
{
	return MainMenuWidget;
}
