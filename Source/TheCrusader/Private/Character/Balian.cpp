// Written by jaegang lim


#include "Character/Balian.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FTCGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Component/Inventory/InventoryComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "Input/TCInputComponent.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Weapon/MyItem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/TCPlayerState.h"
#include "UI/TC_HUD.h"


// Sets default values
ABalian::ABalian()
	: InteractionCheckFrequency(.1f),
	  InteractionCheckDistance(225.f),
	  DesiredSprintSpeed(450),
	  DesiredWalkSpeed(170)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	PlayerInventory->SetSlotsCapacity(20);
	PlayerInventory->SetWeightCapacity(50.f);

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");

	BaseEyeHeight = 74.f;
}

// Called when the game starts or when spawned
void ABalian::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<ATC_HUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	UpdateHealthBar();

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

void ABalian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

void ABalian::OnDamaged(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags,
                        ATCGASCharacter* InstigatorCharacter, AActor* DamageCauser)
{
	Super::OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}

void ABalian::OnHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	UpdateHealthBar();
}

void ABalian::SetIsSprinting(bool bNewIsSprinting)
{
	if (bNewIsSprinting != bIsSprinting)
	{
		bIsSprinting = bNewIsSprinting;
		OnIsSprintingChanged(bNewIsSprinting);
	}
}

void ABalian::EquipToHand() const
{
	if (!CurrentWeapon->bEquipped)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
		                                 CurrentWeapon->GetItemData()->WeaponData.EquipmentSocket);
		CurrentWeapon->bEquipped = true;
	}
}

void ABalian::AttachToPelvis() const
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
		                                 CurrentWeapon->GetItemData()->WeaponData.AttachmentSocket);
		CurrentWeapon->bEquipped = false;
	}
}

bool ABalian::SetAnimLayer(EWeaponType Mode)
{
	switch (Mode)
	{
	case EWeaponType::Idle:
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		HUD->UpdateActiveRadialWidget(0);
		PlayerMode = EWeaponType::Idle;
		if (CurrentWeapon && CurrentWeapon->bEquipped)
			PlayAnimMontage(AttachMontages[CurrentWeapon->GetItemData()->WeaponData.Type]);
		return true;
	case EWeaponType::Boxer:
		GetCharacterMovement()->bOrientRotationToMovement = true;
		HUD->UpdateActiveRadialWidget(1);
		PlayerMode = EWeaponType::Boxer;
		if (CurrentWeapon && CurrentWeapon->bEquipped)
			PlayAnimMontage(AttachMontages[CurrentWeapon->GetItemData()->WeaponData.Type]);
		break;
	case EWeaponType::TwoHandSword:
	case EWeaponType::OneHandSword:
	case EWeaponType::Spear:
	case EWeaponType::Hammer:
		if (!CurrentWeapon)
			return false;
	// 소켓에 부착
		GetCharacterMovement()->bOrientRotationToMovement = true;
		HUD->UpdateActiveRadialWidget(2);
		PlayerMode = CurrentWeapon->GetItemData()->WeaponData.Type;
		PlayAnimMontage(EquipMontages[PlayerMode]);
		break;
	case EWeaponType::Bow:
		break;
	default: ;
	}
	if (AnimLayers.Contains(Mode))
	{
		GetMesh()->LinkAnimClassLayers(AnimLayers[Mode]);
		return true;
	}
	return false;
}

void ABalian::SetCurrentWeapon(AMyItem* Weapon)
{
	if (Weapon)
	{
		if (CurrentWeapon)
		{
			// Pickup 시도 (인벤토리에 넣음)
			CurrentWeapon->Interact(this);
			CurrentWeapon = nullptr;
		}
		CurrentWeapon = Weapon;
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
		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Roll,
		                                        ETriggerEvent::Canceled, this, &ThisClass::Dodge);
		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Roll,
		                                        ETriggerEvent::Triggered, this, &ThisClass::Roll);
	}
}

#pragma region InputBinding

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
	if (HUD->bIsRadialMenuVisible)
		return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

#pragma endregion InputBinding

#pragma region ASCBinding

bool ABalian::ActivateAbilitiesByWeaponType(EWeaponType Mode, bool bAllowRemoteActivation)
{
	const FGameplayAbilitySpecHandle* FoundHandle = MeleeAbilitySpec.Find(Mode);

	if (FoundHandle && AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbility(*FoundHandle, bAllowRemoteActivation);
	}
	return false;
}

void ABalian::DoMeleeAttack()
{
	// 일시정지인지 다른 스킬을 사용하고있는지 살아있는지 확인해야함
	if (UGameplayStatics::IsGamePaused(GetWorld()))
		return;

	FGameplayTagContainer CheckThisTagContainer;
	CheckThisTagContainer.AddTag(FGameplayTag::RequestGameplayTag("Ability.Action.LightAttack"));

	TArray<UTCGameplayAbility*> GameplayAbilities;

	AbilitySystemComponent->GetActiveAbilitiesWithTags(CheckThisTagContainer, GameplayAbilities);

	// 이미 MeleeAttack을 진행중인가?
	if (GameplayAbilities.Num() > 0)
	{
		// 콤보를 업데이트한다.
		JumpSectionForCombo();
	}
	else
	{
		// 새로 몽타주를 재생한다.
		if (ActivateAbilitiesByWeaponType(PlayerMode, true))
		{
			if (PlayerMode == EWeaponType::Idle)
			{
				SetAnimLayer(EWeaponType::Boxer);
			}
		}
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
	// Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Action.LightAttack"));
	// AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
	DoMeleeAttack();
}

void ABalian::RMBClick()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("RMBCLICKED"));
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Action.Block"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
	// AbilitySystemComponent->ApplyGameplayEffectToSelf(HUDMode.GetDefaultObject(), 1,
	//                                                   AbilitySystemComponent->MakeEffectContext());
}

void ABalian::RMBCompleted()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("RMBReleased"));
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("State.Blocking"));
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Container);
}

void ABalian::Dodge()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("LAlt Clicked"));
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
		TEXT("Direction"),
		FVector::ZeroVector,
		UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->GetLastInputVector()));
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Movement.Dodge"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}

void ABalian::Roll()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("LAlt Double Clicked"));

	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
		TEXT("Direction"),
		FVector::ZeroVector,
		UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->GetLastInputVector())
	);
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Movement.Roll"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}

#pragma endregion ASCBinding

#pragma region Interact

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

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);

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
		HUD->HideInteractionWidget();

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

void ABalian::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void ABalian::UpdateHealthBar() const
{
	HUD->SetHP(GetHealth() / GetMaxHealth());
}

#pragma endregion Interact
