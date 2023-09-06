// Written by jaegang lim


#include "Character/Balian.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "Input/TCInputComponent.h"
#include "Player/TCPlayerState.h"


// Sets default values
ABalian::ABalian()
	: DesiredSprintSpeed(450),
	  DesiredWalkSpeed(170)
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ABalian::SetIsSprinting(bool bNewIsSprinting)
{
	if (bNewIsSprinting != bIsSprinting)
	{
		bIsSprinting = bNewIsSprinting;
		OnIsSprintingChanged(bNewIsSprinting);
	}
}

// Called when the game starts or when spawned
void ABalian::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			Subsystem->AddMappingContext(GASMappingContext, 1);
		}
	}
}

void ABalian::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server only
	ATCPlayerState* PS = GetPlayerState<ATCPlayerState>();
	if (PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UTCAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		InitializeAttributes();

		AddStartupEffects();

		AddCharacterAbilities();

		// if (AbilitySystemComponent->GetTagCount(DeadTag) > 0)
		// {
		// 	// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		// 	SetHealth(GetMaxHealth());
		// 	SetStamina(GetMaxStamina());
		// 	SetShield(GetMaxShield());
		// }
	}
}

void ABalian::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client only
	ATCPlayerState* PS = GetPlayerState<ATCPlayerState>();
	if (PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UTCAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &ThisClass::OnAbilityActivationFailed);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		InitializeAttributes();

		// if (AbilitySystemComponent->GetTagCount(DeadTag) > 0)
		// {
		// 	// Set Health/Mana/Stamina/Shield to their max. This is only for *Respawn*. It will be set (replicated) by the
		// 	// Server, but we call it here just to be a little more responsive.
		// 	SetHealth(GetMaxHealth());
		// 	SetStamina(GetMaxStamina());
		// 	SetShield(GetMaxShield());
		// }
	}
}

void ABalian::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABalian::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void ABalian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABalian::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UTCInputComponent* EnhancedInputComponent = CastChecked<UTCInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		EnhancedInputComponent->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag("InputTag.Spacebar"),
		                                        ETriggerEvent::Triggered, this, &ThisClass::SpaceBarClick);
		EnhancedInputComponent->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag("InputTag.LMB"),
		                                        ETriggerEvent::Triggered, this, &ThisClass::LMBClick);
		EnhancedInputComponent->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag("InputTag.RMB"),
		                                        ETriggerEvent::Triggered, this, &ThisClass::RMBClick);
		EnhancedInputComponent->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag("InputTag.RMB"),
												ETriggerEvent::Completed, this, &ThisClass::RMBCompleted);
	}
}

void ABalian::SpaceBarClick()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("Jump"));
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Movement.Jump"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}


void ABalian::LMBClick()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("LMBCLICKED"));
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Attack.C"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}

void ABalian::RMBClick()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("RMBCLICKED"));
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Action.Block"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}

void ABalian::RMBCompleted()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("RMBReleased"));
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("State.Blocking"));
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Container);
}
