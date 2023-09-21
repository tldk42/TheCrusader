// Written by jaegang lim


#include "Player/TCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Character/Balian.h"
#include "Input/TCInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/TC_HUD.h"


bool ATCPlayerController::UpdatePlayerState(const EButtonType Type)
{
	bool Result = false;
	switch (Type)
	{
	case EButtonType::Idle:
		UE_LOG(LogTemp, Display, TEXT("Idle"));
		Result = PlayerRef->SetAnimLayer(EWeaponType::Idle);
		break;
	case EButtonType::Fist:
		UE_LOG(LogTemp, Display, TEXT("Fist"));
		Result = PlayerRef->SetAnimLayer(EWeaponType::Boxer);
		break;
	case EButtonType::Sword:
		UE_LOG(LogTemp, Display, TEXT("Sword"));
		Result = PlayerRef->SetAnimLayer(EWeaponType::TwoHandSword);
		break;
	case EButtonType::Bow:
		UE_LOG(LogTemp, Display, TEXT("Bow"));
		break;
	case EButtonType::Horse:
		UE_LOG(LogTemp, Display, TEXT("Horse"));
		break;
	case EButtonType::Torch:
		UE_LOG(LogTemp, Display, TEXT("Torch"));
		break;
	case EButtonType::Locked:
		UE_LOG(LogTemp, Display, TEXT("Locked"));
		break;
	default: ;
	}

	return Result;
}

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
		EnhancedInputComponent->BindAction(RadialMenuAction, ETriggerEvent::Triggered, this,
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
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), .05f);
}

void ATCPlayerController::CloseRadialMenu()
{
	HUD->HideRadialWidget();
	SetInputMode(FInputModeGameOnly());
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}
