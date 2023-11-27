// Written by jaegang lim


#include "Character/EnemyBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/Balian.h"
#include "Character/AI/AIController_Base.h"
#include "Component/Physics/TCPhysicalAnimComp.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "GAS/Attribute/TCAttributeSet.h"
#include "Item/Weapon/Item_Weapon.h"
#include "UI/DamageText/DamageText.h"
#include "UI/FloatingBar/EnemyBar.h"


AEnemyBase::AEnemyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
	PrimaryActorTick.bCanEverTick = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	AIControllerClass = AAIController_Base::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	FloatingWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("W_Status"));
	FloatingWidgetComponent->SetRelativeLocation(FVector(0, 0, 190.f));
	FloatingWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingWidgetComponent->SetDrawAtDesiredSize(true);

	LockWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("W_LockOn"));
	LockWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockWidgetComponent->SetDrawAtDesiredSize(false);

	StealthLocation = CreateDefaultSubobject<USceneComponent>(TEXT("L_Stealth"));
	FloatingWidgetComponent->SetupAttachment(GetRootComponent());
	LockWidgetComponent->SetupAttachment(GetRootComponent());
	StealthLocation->SetupAttachment(GetRootComponent());

	AbilitySystemComponent = CreateDefaultSubobject<UTCAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSetBase = CreateDefaultSubobject<UTCAttributeSet>("AttributeSet");
}

void AEnemyBase::BeginFocus()
{
	IInteractable::BeginFocus();
}

void AEnemyBase::EndFocus()
{
	IInteractable::EndFocus();
}

void AEnemyBase::BeginInteract()
{
	IInteractable::BeginInteract();
}

void AEnemyBase::EndInteract()
{
	IInteractable::EndInteract();
}

void AEnemyBase::Interact(ABalian* PlayerCharacter)
{
	IInteractable::Interact(PlayerCharacter);

	PlayerCharacter->StealthTakeDown();
}

void AEnemyBase::SetupStartupWeapon(AItem_Weapon* Weapon)
{
	CurrentWeapon = Weapon;

	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "sword");
	CurrentWeapon->DisableInteractionCollision();
	CurrentWeapon->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CurrentWeapon->SetInstigator(this);
	CombatMode = EWeaponType::SwordWithShield;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	InteractableData = InstancedInteractableData;

	InitializeAttributes();
	AddStartupEffects();
	AddCharacterAbilities();

	StatusWidget = CreateWidget<UEnemyBar>(GetWorld(), FloatingWidgetClass);
	LockWidget = CreateWidget<UUserWidget>(GetWorld(), LockWidgetClass);
	FloatingWidgetComponent->SetWidget(StatusWidget);
	LockWidgetComponent->SetWidget(LockWidget);

	StatusWidget->SetVisibility(ESlateVisibility::Collapsed);
	LockWidget->SetVisibility(ESlateVisibility::Collapsed);

	UpdateHealthBar();
	UpdateStaminaBar();
	HideFloatingBar();

	if (WeaponClass)
	{
		AItem_Weapon* Weapon = GetWorld()->SpawnActor<AItem_Weapon>(WeaponClass);
		if (Weapon)
		{
			SetupStartupWeapon(Weapon);
		}
	}
}

void AEnemyBase::Die()
{
	Super::Die();

	OnUnLocked();
}

void AEnemyBase::ChangeCharacterState(const ECharacterState NewState)
{
	Super::ChangeCharacterState(NewState);

	if (AAIController_Base* AIController = Cast<AAIController_Base>(GetController()))
	{
		AIController->GetBlackboardComponent()->SetValueAsEnum(AAIController_Base::KEY_STATE,
		                                                       static_cast<uint8>(CharacterState));
	}
}

void AEnemyBase::UpdateHealthBar() const
{
	if (StatusWidget)
	{
		StatusWidget->UpdateHP(GetHealth() / GetMaxHealth());
	}
}

void AEnemyBase::UpdateStaminaBar() const
{
	if (StatusWidget)
	{
		StatusWidget->UpdateStamina(GetStamina() / GetMaxStamina());
	}
}

void AEnemyBase::OnDamaged(const float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags,
                           ATCGASCharacter* InstigatorCharacter, AActor* DamageCauser)
{
	Super::OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (FVector2D PathToScreen; UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController,
		HitInfo.Location.IsZero() ? GetActorLocation() : HitInfo.Location, PathToScreen, false))
	{
		if (UDamageText* DamageTextWidget = CreateWidget<UDamageText>(GetWorld(), DamageTextWidgetClass))
		{
			const FSlateColor HardTextColor(FColor::Red);
			DamageTextWidget->DamageText->SetText(FText::AsNumber(FMath::FloorToInt(DamageAmount)));
			if (DamageAmount > 49.f)
			{
				DamageTextWidget->DamageText->SetColorAndOpacity(HardTextColor);
			}
			UWidgetLayoutLibrary::SlotAsCanvasSlot(DamageTextWidget->DamageText)->SetPosition(PathToScreen);
			DamageTextWidget->AddToViewport();
		}
	}


	// SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DamageCauser->GetActorLocation()));

	FTimerHandle StateResetTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StateResetTimerHandle,
	                                       [this]() { ChangeCharacterState(ECharacterState::Combat); },
	                                       DamageTags.HasTagExact(
		                                       FGameplayTag::RequestGameplayTag("Ability.Action.Parry"))
		                                       ? 2.f
		                                       : 1.f,
	                                       false);
	ChangeCharacterState(ECharacterState::Shocked);

	PhysicalAnimComponent->HitReaction(HitInfo);
	if (!TargetCharacter)
	{
		TargetCharacter = InstigatorCharacter;
	}
}

void AEnemyBase::OnLocked() const
{
	ShowFloatingBar();
	LockWidget->SetVisibility(ESlateVisibility::Visible);
	StatusWidget->HighlightBorder();
}

void AEnemyBase::OnUnLocked() const
{
	HideFloatingBar();
	LockWidget->SetVisibility(ESlateVisibility::Collapsed);
	StatusWidget->UnHighlightBorder();
}

void AEnemyBase::ShowFloatingBar() const
{
	StatusWidget->SetVisibility(ESlateVisibility::Visible);
}

void AEnemyBase::HideFloatingBar() const
{
	StatusWidget->SetVisibility(ESlateVisibility::Collapsed);
}
