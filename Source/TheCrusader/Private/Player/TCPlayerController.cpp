// Written by jaegang lim


#include "Player/TCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Character/Balian.h"
#include "Input/TCInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/TC_HUD.h"


void ATCPlayerController::ToggleMenu()
{
	HUD->ToggleMenu();
}

void ATCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerRef = Cast<ABalian>(GetCharacter());
	HUD = Cast<ATC_HUD>(GetHUD());

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ControllerMappingContext, 0);
	}
}

void ATCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UTCInputComponent* EnhancedInputComponent = CastChecked<UTCInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(RadialMenuAction, ETriggerEvent::Started, this,
		                                   &ATCPlayerController::ShowRadialMenu);
		EnhancedInputComponent->BindAction(RadialMenuAction, ETriggerEvent::Completed, this,
		                                   &ThisClass::CloseRadialMenu);
		EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Triggered, this, &ThisClass::ToggleMenu);
	}
}

void ATCPlayerController::ShowRadialMenu()
{
	HUD->ShowRadialWidget();
	SetInputMode(FInputModeGameAndUI());
		OriginTimeDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), .05f);
}

void ATCPlayerController::CloseRadialMenu()
{
	HUD->HideRadialWidget();
	SetInputMode(FInputModeGameOnly());
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), OriginTimeDilation);
}
