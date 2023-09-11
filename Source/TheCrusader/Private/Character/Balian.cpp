// Written by jaegang lim


#include "Character/Balian.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FTCGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "Input/TCInputComponent.h"
#include "Player/TCPlayerState.h"


// Sets default values
ABalian::ABalian()
	: DesiredSprintSpeed(450),
	  DesiredWalkSpeed(170),
	  InteractionCheckFrequency(.1f),
	  InteractionCheckDistance(225.f)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	BaseEyeHeight = 74.f;
}

void ABalian::SetIsSprinting(bool bNewIsSprinting)
{
	if (bNewIsSprinting != bIsSprinting)
	{
		bIsSprinting = bNewIsSprinting;
		OnIsSprintingChanged(bNewIsSprinting);
	}
}

void ABalian::SetPlayerMode(EPlayerMode Mode)
{
	switch (Mode)
	{
	case EPlayerMode::Idle:
		// GetCameraBoom()->bUsePawnControlRotation = true;
		// bUseControllerRotationYaw = false;
		// GetCharacterMovement()->bOrientRotationToMovement = false;
		PlayerMode = EPlayerMode::Idle;
		break;
	case EPlayerMode::Boxer:
		// GetCharacterMovement()->bUseControllerDesiredRotation = true;
		// GetCameraBoom()->bUsePawnControlRotation = false;
		// bUseControllerRotationYaw = true;
		PlayerMode = EPlayerMode::Boxer;
		break;
	case EPlayerMode::TwoHandSword:
		break;
	case EPlayerMode::OneHandSword:
		break;
	case EPlayerMode::Spear:
		break;
	case EPlayerMode::Hammer:
		break;
	case EPlayerMode::Bow:
		break;
	default: ;
	}

	GetMesh()->LinkAnimClassLayers(AnimLayers[Mode]);
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

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

void ABalian::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UTCInputComponent* EnhancedInputComponent = CastChecked<UTCInputComponent>(PlayerInputComponent))
	{
		const FTCGameplayTags& GameplayTags = FTCGameplayTags::Get();

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Spacebar,
		                                        ETriggerEvent::Triggered, this, &ThisClass::SpaceBarClick);
		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_LMB,
		                                        ETriggerEvent::Triggered, this, &ThisClass::LMBClick);
		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_RMB,
		                                        ETriggerEvent::Triggered, this, &ThisClass::RMBClick);
		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_RMB,
		                                        ETriggerEvent::Completed, this, &ThisClass::RMBCompleted);
		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_E,
		                                        ETriggerEvent::Triggered, this, &ThisClass::BeginInteract);
		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_E,
		                                        ETriggerEvent::Completed, this, &ThisClass::EndInteract);
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
	// FGameplayTagContainer Container;
	// Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Attack.C"));
	// AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
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

void ABalian::PerformInteractionCheck()
{
	// 주기 업데이트
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	const FVector TraceStart{GetPawnViewLocation()};
	FVector TraceEnd{TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance)};

	// Player dot Controller
	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	// 플레이어 방향과 바라보는 방향이 90도 이하일 때만 Trace를 할 것.
	if (LookDirection > 0)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Orange, false, 1.f, 0, 1.f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult TraceHit;

		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			if (TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
					return;

				FoundInteractable(TraceHit.GetActor());
				return;
			}
		}
	}

	NoInteractableFound();
}

void ABalian::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	TargetInteractable->BeginFocus();
}

void ABalian::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}

		// Hide Interaction Widget on the HUD

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void ABalian::BeginInteract()
{
	// 동일한 것을 보고있는지 확인하는 안전장치(변한게 있나?)
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, .1f))
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction, this, &ThisClass::Interact,
				                                TargetInteractable->InteractableData.InteractionDuration, false);
			}
		}
	}
}

void ABalian::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void ABalian::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}
