// Written by jaegang lim


#include "Character/Balian.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FTCGameplayTags.h"
#include "Component/Inventory/InventoryComponent.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "Input/TCInputComponent.h"
#include "MotionWarpingComponent.h"
#include "Character/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Data/ItemEquipmentBase.h"
#include "Item/Weapon/Item_Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/TCPlayerState.h"
#include "UI/TC_HUD.h"
#include "UI/Radial/RadialButtonBase.h"
#include "Character/Horse_Base.h"
#include "Character/InventoryPreview.h"
#include "Character/Movement/TCMovementComponent.h"
#include "Component/Skill/SkillComponent.h"
#include "Game/TheCrusaderGameMode.h"
#include "GAS/Attribute/TCAttributeSet.h"
#include "Item/Weapon/Item_Weapon_Bow.h"
#include "Kismet/GameplayStatics.h"
#include "Game/LoadScreenSaveGame.h"
#include "Player/TCPlayerController.h"


ABalian::ABalian(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<
		  UTCMovementComponent>(ACharacter::CharacterMovementComponentName)),
	  InteractionCheckFrequency(.1f),
	  InteractionCheckDistance(225.f),
	  DesiredSprintSpeed(450),
	  DesiredWalkSpeed(170)
{
	PrimaryActorTick.bCanEverTick = true;

	TCMovementComponent = Cast<UTCMovementComponent>(GetCharacterMovement());

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetSlotsCapacity(20);
	InventoryComponent->SetWeightCapacity(50.f);

	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));

	BaseEyeHeight = 74.f;
}

void ABalian::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<ATC_HUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	SpawnPreviewBalian();
	UpdateHealthBar();
	UpdateStaminaBar();

	LoadProgress();
}

void ABalian::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);


	// TODO: Ability 초기화 구분 필요

	// Server only
	ATCPlayerState* PS = GetPlayerState<ATCPlayerState>();
	if (PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UTCAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		if (!AbilitySystemComponent->bStartupEffectsApplied)
		{
			// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
			PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

			// Set the AttributeSetBase for convenience attribute functions
			AttributeSetBase = PS->GetAttributeSetBase();

			InitializeAttributes();

			AddStartupEffects();

			AddCharacterAbilities();

			if (AbilitySystemComponent->GetTagCount(DeadTag) > 0)
			{
				// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
				AttributeSetBase->SetHealth(GetMaxHealth());
				AttributeSetBase->SetStamina(GetMaxStamina());
			}
		}
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

		if (AbilitySystemComponent->GetTagCount(DeadTag) > 0)
		{
			// Set Health/Mana/Stamina/Shield to their max. This is only for *Respawn*. It will be set (replicated) by the
			// Server, but we call it here just to be a little more responsive.
			AttributeSetBase->SetHealth(GetMaxHealth());
			AttributeSetBase->SetStamina(GetMaxStamina());
		}
	}
}

void ABalian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (!bRiding && GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}

	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Aiming")))
	{
		if (CurrentBow)
		{
			CurrentBow->PredictTarget();
			AimingDuration += DeltaTime;
		}
	}

	FocusCameraToTarget();
}

void ABalian::OnDamaged(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags,
                        ATCGASCharacter* InstigatorCharacter, AActor* DamageCauser)
{
	Super::OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}

void ABalian::SetIsSprinting(bool bNewIsSprinting)
{
	if (bNewIsSprinting != bIsSprinting)
	{
		bIsSprinting = bNewIsSprinting;
		OnIsSprintingChanged(bNewIsSprinting);
	}
}

void ABalian::SetCurrentWeapon(AItem_Weapon* Weapon)
{
	float WeaponDamage = 0;
	if (CurrentWeapon)
	{
		WeaponDamage -= CurrentWeapon->GetItemData()->ItemData.WeaponData.BaseDamage;

		if (Weapon)
		{
			const EWeaponType NewType = Weapon->GetItemData()->ItemData.WeaponData.Type;
			WeaponDamage += Weapon->GetItemData()->ItemData.WeaponData.BaseDamage;

			if (CurrentWeapon->bEquipped)
			{
				CombatMode = NewType;
			}

			// Pickup 시도 (인벤토리에 넣음)
			CurrentWeapon->Interact(this);
			CurrentWeapon = nullptr;
			CurrentWeapon = Weapon;
		}
		else
		{
			CurrentWeapon->Interact(this);
			CurrentWeapon = nullptr;

			if (CombatMode != EWeaponType::None)
			{
				CombatMode = EWeaponType::Boxer;
				HUD->UpdateActiveRadialWidget(1);
			}
		}
	}
	else
	{
		if (Weapon)
		{
			CurrentWeapon = Weapon;
			WeaponDamage = Weapon->GetItemData()->ItemData.WeaponData.BaseDamage;
		}
	}

	SetAnimLayer();

	if (Weapon)
	{
		PreviewCharacter->AttachEquipment(EEquipmentPart::Weapon, Weapon->GetItemData());
	}
	else
	{
		PreviewCharacter->DetachEquipment(EEquipmentPart::Weapon);
	}

	AttributeSetBase->SetAttackPower(AttributeSetBase->GetAttackPower() + WeaponDamage);
}

void ABalian::SetCurrentBow(AItem_Weapon_Bow* Bow)
{
	if (CurrentBow)
	{
		if (Bow)
		{
			CurrentBow->Interact(this);
			CurrentBow = nullptr;
			CurrentBow = Bow;
		}
	}
	else
	{
		if (Bow)
		{
			CurrentBow = Bow;
		}
		else
		{
			CurrentBow->Interact(this);
			CurrentBow = nullptr;
		}

		if (CurrentBow)
		{
			CurrentBow->GetSkeletalMeshComp()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	if (Bow)
	{
		PreviewCharacter->AttachEquipment(EEquipmentPart::Bow, Bow->GetItemData());
	}
	else
	{
		PreviewCharacter->DetachEquipment(EEquipmentPart::Bow);
	}
}

void ABalian::AttachEquipment(const EEquipmentPart EquipmentPart, const UItemEquipmentBase* ItemToEquip)
{
	Super::AttachEquipment(EquipmentPart, ItemToEquip);

	PreviewCharacter->AttachEquipment(EquipmentPart, ItemToEquip);
}

void ABalian::DetachEquipment(EEquipmentPart EquipmentPart)
{
	Super::DetachEquipment(EquipmentPart);

	PreviewCharacter->DetachEquipment(EquipmentPart);


	// TODO: + 능력치 조정
	GetInventory()->DettachEquipmentItem(EquipmentPart);
}

void ABalian::SetOwningHorse(AHorse_Base* HorseToRide)
{
	OwningHorse = HorseToRide;
}

void ABalian::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// check(PlayerInputComponent);

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 2);
			Subsystem->AddMappingContext(GASMappingContext, 3);
		}
	}

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
		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_MouseMiddle,
		                                        ETriggerEvent::Triggered, this, &ThisClass::MMBClick);
		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Numpad1,
		                                        ETriggerEvent::Triggered, this, &ThisClass::Ability1);
		EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Crouch,
		                                        ETriggerEvent::Triggered, this, &ThisClass::CrouchPressed);
	}
}

void ABalian::RemoveMappingContext() const
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
			Subsystem->RemoveMappingContext(GASMappingContext);
		}
	}
}

int32 ABalian::GetStatPoints_Implementation() const
{
	return ISavableCharacter::GetStatPoints_Implementation();
}

int32 ABalian::GetSkillPoints_Implementation() const
{
	return ISavableCharacter::GetSkillPoints_Implementation();
}

void ABalian::LevelUp_Implementation()
{
	ISavableCharacter::LevelUp_Implementation();
}

void ABalian::ShowSaveWidget_Implementation() const
{
	if (ATCPlayerController* PlayerController = Cast<ATCPlayerController>(GetController()))
	{
		// PlayerController.ShowSaveWidget()
	}
}

void ABalian::HideSaveWidget_Implementation() const
{
	if (ATCPlayerController* PlayerController = Cast<ATCPlayerController>(GetController()))
	{
		// PlayerController.HideSaveWidget()
	}
}

void ABalian::SaveProgress_Implementation(const FName& CheckpointTag)
{
	if (const ATheCrusaderGameMode* TCGameMode = Cast<ATheCrusaderGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (ULoadScreenSaveGame* SaveData = TCGameMode->RetrieveInGameSaveData())
		{
			SaveData->PlayerStartTag = CheckpointTag;

			if (ATCPlayerState* TCPlayerState = Cast<ATCPlayerState>(GetPlayerState()))
			{
				// SaveData->Strength = TCPlayerState
				SaveData->PlayerStartLocation = GetActorLocation();
				SaveData->Health = AttributeSetBase->GetHealth();
				SaveData->Stamina = AttributeSetBase->GetStamina();
				SaveData->AttackPower = AttributeSetBase->GetAttackPower();
				SaveData->DefensePower = AttributeSetBase->GetDefensePower();
				SaveData->SkillPower = AttributeSetBase->GetSkillPower();
				SaveData->bFirstTimeLoadIn = false;
			}

			if (InventoryComponent)
			{
				SaveData->SavedInventory.Empty();
				SaveData->SavedEquipments.Empty();

				for (UItemBase* ItemBase : InventoryComponent->GetInventoryContents())
				{
					FSavedInventory SavedInventory;
					SavedInventory.InventoryItem = ItemBase->ItemData;
					SavedInventory.ItemDataClass = ItemBase->GetClass();
					SaveData->SaveData(ItemBase, SavedInventory.Data);

					SaveData->SavedInventory.Add(SavedInventory);
				}
				for (const auto& EquippedContent : InventoryComponent->GetEquippedContents())
				{
					FSavedInventory SavedInventory;
					SaveData->SaveData(EquippedContent.Value, SavedInventory.Data);
					SavedInventory.InventoryItem = EquippedContent.Value->ItemData;
					SavedInventory.ItemDataClass = EquippedContent.Value->GetClass();

					SaveData->SavedEquipments.Add(SavedInventory);
				}
			}

			const UWorld* World = GetWorld();
			FString MapName = World->GetMapName();
			MapName.RemoveFromStart(World->StreamingLevelsPrefix);
			TCGameMode->SaveWorldState(GetWorld(), MapName);

			TCGameMode->SaveInGameProgressData(SaveData);
		}
	}
}

void ABalian::LoadProgress()
{
	if (const ATheCrusaderGameMode* TCGameMode = Cast<ATheCrusaderGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (ULoadScreenSaveGame* SaveData = TCGameMode->RetrieveInGameSaveData())
		{
			if (SaveData->bFirstTimeLoadIn)
			{
				// TODO: 기본 초기 정보로 초기화
			}
			else
			{
				// TODO: 저장된 정보로 초기화
				AttributeSetBase->SetHealth(SaveData->Health);
				AttributeSetBase->SetStamina(SaveData->Stamina);
				AttributeSetBase->SetAttackPower(SaveData->AttackPower);
				AttributeSetBase->SetDefensePower(SaveData->DefensePower);
				AttributeSetBase->SetSkillPower(SaveData->SkillPower);
				SetActorLocation(SaveData->PlayerStartLocation);

				if (InventoryComponent)
				{
					for (FSavedInventory& SavedItem : SaveData->SavedInventory)
					{
						UItemBase* Item = NewObject<UItemBase>(this, SavedItem.ItemDataClass);
						Item->ItemData = SavedItem.InventoryItem;
						Item->OwningInventory = this->GetInventory();
						SaveData->LoadData(Item, SavedItem.Data);

						InventoryComponent->HandleAddItem(Item);
					}

					for (FSavedInventory& SavedEquipment : SaveData->SavedEquipments)
					{
						UItemEquipmentBase* Item = NewObject<UItemEquipmentBase>(this, SavedEquipment.ItemDataClass);
						Item->ItemData = SavedEquipment.InventoryItem;
						Item->OwningInventory = this->GetInventory();
						SaveData->LoadData(Item, SavedEquipment.Data);
						Item->MoveToEquipment(this);
					}
				}
				TCGameMode->LoadWorldState(GetWorld());
			}
		}
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

		if (MovementVector.Y >= 0.6f)
		{
			InputDirection = ECardinalDirection::Forward;
		}
		else if (MovementVector.X >= 0.5f)
		{
			InputDirection = ECardinalDirection::Right;
		}
		else if (MovementVector.X <= -0.5f)
		{
			InputDirection = ECardinalDirection::Left;
		}
		else
		{
			InputDirection = ECardinalDirection::Backward;
		}

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
	return Super::ActivateAbilitiesByWeaponType(Mode, bAllowRemoteActivation);
}

void ABalian::FocusCameraToTarget()
{
	if (bIsTargeting)
	{
		if (TargetCharacter)
		{
			if (const float TargetDistance = GetDistanceTo(TargetCharacter); !TargetCharacter->IsAlive() ||
				TargetDistance > 1000.f)
			{
				ReleaseCamera();
			}
			else
			{
				const FVector TargetLoc = TargetCharacter->GetActorLocation();
				const FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(
					GetActorLocation(), FVector(TargetLoc.X, TargetLoc.Y, TargetLoc.Z - 50.f));

				const float TargetPitch = FMath::Lerp(GetControlRotation(), TargetRot, .6f).Pitch;

				const FRotator TargetRotator(TargetPitch, TargetRot.Yaw, 0.f);
				const FRotator Rotator = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotator,
				                                                       GetWorld()->GetDeltaSeconds(), 4.5f);


				GetController()->SetControlRotation(Rotator);
			}
		}
		else
		{
			ReleaseCamera();
		}
	}
}

void ABalian::EquipToHand(const bool bMelee)
{
	if (bMelee)
	{
		if (CurrentWeapon && !CurrentWeapon->bEquipped)
		{
			CurrentWeapon->bEquipped = true;

			FOnMontageBlendingOutStarted BlendingOutDelegate
				= FOnMontageBlendingOutStarted::CreateLambda([this](UAnimMontage*, bool)
				{
					if (CurrentWeapon)
						CurrentWeapon->AttachToComponent(
							GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
							CurrentWeapon->GetItemData()->ItemData.EquipmentData.AttachmentSocket);
				});


			PlayAnimMontage(EquipMontages[CombatMode]);

			GetMesh()->GetAnimInstance()->
			           Montage_SetBlendingOutDelegate(BlendingOutDelegate, EquipMontages[CombatMode]);
		}
	}
	else
	{
		if (CurrentBow && !CurrentBow->bEquipped)
		{
			CurrentBow->bEquipped = true;

			FOnMontageBlendingOutStarted BlendingOutDelegate
				= FOnMontageBlendingOutStarted::CreateLambda([this](UAnimMontage*, bool)
				{
					if (CurrentBow)
						CurrentBow->AttachToComponent(
							GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
							CurrentBow->GetItemData()->ItemData.EquipmentData.AttachmentSocket);
				});


			PlayAnimMontage(EquipMontages[CombatMode]);

			GetMesh()->GetAnimInstance()->
			           Montage_SetBlendingOutDelegate(BlendingOutDelegate, EquipMontages[CombatMode]);
		}
	}
}

void ABalian::AttachToPelvis(const bool bMelee)
{
	if (bMelee)
	{
		if (CurrentWeapon && CurrentWeapon->bEquipped)
		{
			CurrentWeapon->bEquipped = false;

			FOnMontageBlendingOutStarted BlendingOutDelegate
				= FOnMontageBlendingOutStarted::CreateLambda([this](UAnimMontage*, bool)
				{
					if (CurrentWeapon)
						CurrentWeapon->AttachToComponent(
							GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
							CurrentWeapon->GetItemData()->ItemData.EquipmentData.DetachmentSocket);
				});


			PlayAnimMontage(UnEquipMontages[CurrentWeapon->GetItemData()->ItemData.WeaponData.Type]);

			GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendingOutDelegate,
			                                                             UnEquipMontages[CurrentWeapon->GetItemData()->
				                                                             ItemData.WeaponData.Type]);
		}
	}
	else
	{
		if (CurrentBow && CurrentBow->bEquipped)
		{
			CurrentBow->bEquipped = false;

			FOnMontageBlendingOutStarted BlendingOutDelegate
				= FOnMontageBlendingOutStarted::CreateLambda([this](UAnimMontage*, bool)
				{
					if (CurrentBow)
						CurrentBow->AttachToComponent(
							GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
							CurrentBow->GetItemData()->ItemData.EquipmentData.DetachmentSocket);
				});


			PlayAnimMontage(UnEquipMontages[EWeaponType::Bow]);

			GetMesh()->GetAnimInstance()->
			           Montage_SetBlendingOutDelegate(BlendingOutDelegate, UnEquipMontages[EWeaponType::Bow]);
		}
	}
}

void ABalian::SetVisibility_Accessory() const
{
	HandArrow->SetVisibility(false);
	Quiver->SetVisibility(false);

	if (CurrentWeapon && CurrentWeapon->GetItemData()->ItemData.WeaponData.Type == EWeaponType::OneHandSword)
	{
		SwordZip->SetVisibility(true);
		LongswordZip->SetVisibility(false);
		if (CombatMode == EWeaponType::OneHandSword)
		{
			ShieldMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			                              ("lowerarm_lSocket"));
		}
		else
		{
			ShieldMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			                              TEXT("shield_equip"));
		}
	}
	else if (CurrentWeapon && CurrentWeapon->GetItemData()->ItemData.WeaponData.Type == EWeaponType::TwoHandSword)
	{
		SwordZip->SetVisibility(false);
		LongswordZip->SetVisibility(true);
		ShieldMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
		                              TEXT("shield_equip"));
	}
	else
	{
		SwordZip->SetVisibility(false);
		LongswordZip->SetVisibility(false);
		ShieldMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
		                              TEXT("shield_equip"));
	}

	if (CurrentBow && CombatMode == EWeaponType::Bow)
	{
		Quiver->SetVisibility(true);
		HandArrow->SetVisibility(true);
	}
}

void ABalian::LockCamera()
{
	if (TargetCharacter)
	{
		if (const AEnemyBase* Enemy = Cast<AEnemyBase>(TargetCharacter))
		{
			bIsTargeting = true;
			Enemy->OnLocked();
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
}

void ABalian::ReleaseCamera()
{
	bIsTargeting = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	if (const AEnemyBase* Enemy = Cast<AEnemyBase>(TargetCharacter))
	{
		Enemy->OnUnLocked();
	}
	TargetCharacter = nullptr;
}

void ABalian::SpaceBarClick()
{
	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString(L"점프 시작"));
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Movement.Jump"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}


void ABalian::LMBClick()
{
	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString(L"LMB 클릭"));
	if (bCanCounterAttack)
	{
		// CounterAttack;
		bCounterInputTriggered = true;
	}
	else
	{
		if (CombatMode == EWeaponType::Bow)
		{
			DoShoot();
		}
		else if (!bRiding)
		{
			DoMeleeAttack();
		}
	}
}

void ABalian::RMBClick()
{
	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString(L"RMB 클릭"));
	FGameplayTagContainer Container;
	if (CombatMode == EWeaponType::Bow)
	{
		Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Movement.Aiming"));
	}
	else if (!bRiding)
	{
		Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Action.Block"));
		// Apply Parry Effect
	}
	if (AbilitySystemComponent->TryActivateAbilitiesByTag(Container))
	{
		if (CombatMode == EWeaponType::Bow)
		{
			HUD->ShowCrosshair();
		}
	}
}

void ABalian::RMBCompleted()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString(L"RMB 해제"));
	FGameplayTagContainer Container;
	if (CombatMode == EWeaponType::Bow)
	{
		Container.AddTag(FGameplayTag::RequestGameplayTag("State.Aiming"));
		HUD->HideCrosshair();
	}
	else
	{
		Container.AddTag(FGameplayTag::RequestGameplayTag("State.Blocking"));
	}

	// if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Parry")))
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Container);
}

void ABalian::MMBClick()
{
	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString(L"마우스 중간 클릭"));
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Gameplay.FindTarget"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}

void ABalian::Dodge()
{
	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString(L"LAlt 클릭"));

	const FRotator Rotator = GetCharacterMovement()->GetLastInputVector().Rotation();
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
		TEXT("Direction"),
		FVector::ZeroVector,
		Rotator.IsZero() ? GetActorForwardVector().Rotation() : Rotator
	);
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Movement.Dodge"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}

void ABalian::Roll()
{
	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("LAlt 더블 클릭"));

	const FRotator Rotator = GetCharacterMovement()->GetLastInputVector().Rotation();
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
		TEXT("Direction"),
		FVector::ZeroVector,
		Rotator.IsZero() ? GetActorForwardVector().Rotation() : Rotator
	);
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Movement.Roll"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}

void ABalian::Ability1()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("Ability 1 시도"));

	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Skill.FireBurst"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}

void ABalian::CrouchPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("Crouch 시도"));

	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Movement.Crouch"));
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

	// 플레이어 방향과 바라보는 방향이 90도 이하일 때만 Trace를 할 것.
	if (float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector()); LookDirection >=
		-.15f)
	{
		// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Orange, false, 1.f, 0, 1.f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		// Trace Channel : Item 대상으로만 
		if (FHitResult TraceHit; GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd,
		                                                              ECollisionChannel::ECC_GameTraceChannel2,
		                                                              QueryParams))
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

	if (NewInteractable->IsA(AEnemyBase::StaticClass()))
	{
		// TODO: 암살 조건 추가
		// Enemy State 변수 선언
		// State가 Invesgating || Idle || Patrolling 상태일 때 가능하도록 설정
		if (UKismetMathLibrary::Dot_VectorVector(GetActorForwardVector(), NewInteractable->GetActorForwardVector()) >=
			0.5f)
		{
			ATCGASCharacter* CurrentFocusingCharacter = Cast<AEnemyBase>(NewInteractable);

			HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
			TargetCharacter = CurrentFocusingCharacter;
		}
	}
	else
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}


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
		HUD->HideInteractionWidget();
	}
}

void ABalian::SpawnPreviewBalian()
{
	PreviewCharacter =
		GetWorld()->SpawnActor<AInventoryPreview>(PreviewCharacterClass,
		                                          FVector(-24.905944, 7569.893602, 120),
		                                          FRotator::ZeroRotator);
	PreviewCharacter->UpdateAllMesh(this);
}

void ABalian::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void ABalian::SetAnimLayer()
{
	EWeaponType WeaponType = EWeaponType::None;
	if (CurrentWeapon)
	{
		WeaponType = CurrentWeapon->GetItemData()->ItemData.WeaponData.Type;
	}

	// 초기화
	if (CurrentWeapon && CurrentWeapon->bEquipped && UnEquipMontages.Contains(WeaponType))
	{
		AttachToPelvis(true);
	}
	if (CurrentBow && CurrentBow->bEquipped && UnEquipMontages.Contains(EWeaponType::Bow))
	{
		AttachToPelvis(false);
	}

	SetVisibility_Accessory();

	if (EquipMontages.Contains(CombatMode))
	{
		EquipToHand(CombatMode == EWeaponType::Bow ? false : true);
	}

	if (AnimLayers.Contains(CombatMode))
	{
		GetMesh()->LinkAnimClassLayers(AnimLayers[CombatMode]);
	}
}


bool ABalian::UpdateStateByButton(const EButtonType BtnType)
{
	GetCharacterMovement()->bOrientRotationToMovement = true;

	switch (BtnType)
	{
	case EButtonType::Idle:
		GetCharacterMovement()->bOrientRotationToMovement = false;
		CombatMode = EWeaponType::None;
		break;
	case EButtonType::Fist:
		CombatMode = EWeaponType::Boxer;
		break;
	case EButtonType::Sword:
		if (!CurrentWeapon)
			return false;
		CombatMode = CurrentWeapon->GetItemData()->ItemData.WeaponData.Type;
		break;
	case EButtonType::Bow:
		if (!CurrentBow)
			return false;
		CombatMode = EWeaponType::Bow;
		break;
	case EButtonType::Torch:
	case EButtonType::Horse:
		if (OwningHorse)
		{
			OwningHorse->MoveToPlayer(GetActorLocation());
		}
	// Call Horse
		return false;
	default: ;
	}

	SetAnimLayer();

	if (bIsTargeting)
	{
		LockCamera();
	}

	return true;
}

void ABalian::UpdateHealthBar() const
{
	HUD->SetHP(GetHealth() / GetMaxHealth());
}

void ABalian::UpdateStaminaBar() const
{
	HUD->SetStamina(GetStamina() / GetMaxStamina());
}


#pragma endregion Interact
