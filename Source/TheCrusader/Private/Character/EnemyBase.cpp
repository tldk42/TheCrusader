// Written by jaegang lim


#include "Character/EnemyBase.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/Balian.h"
#include "Component/Physics/TCPhysicalAnimComp.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "GAS/Attribute/TCAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/DamageText/DamageText.h"
#include "UI/FloatingBar/EnemyBar.h"


AEnemyBase::AEnemyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
	PrimaryActorTick.bCanEverTick = false;

	FloatingWidget = CreateDefaultSubobject<UWidgetComponent>("FloatingWidget");
	FloatingWidget->SetupAttachment(GetMesh(), FName("root"));
	FloatingWidget->SetRelativeLocation(FVector(0, 0, 190.f));
	FloatingWidget->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingWidget->SetDrawAtDesiredSize(true);

	StealthLocation = CreateDefaultSubobject<USceneComponent>("Stealth Location");
	FloatingWidget->SetupAttachment(GetRootComponent());
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

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	InteractableData = InstancedInteractableData;

	InitializeAttributes();
	AddStartupEffects();
	AddCharacterAbilities();

	InstancedWidget = CreateWidget<UEnemyBar>(GetWorld(), WidgetClass);
	FloatingWidget->SetWidget(InstancedWidget);
	// FloatingWidget->SetVisibility(false);
	UpdateHealthBar();
	HideFloatingBar();
}

void AEnemyBase::Die()
{
	Super::Die();

	UnHighlightBorder();
}

void AEnemyBase::UpdateHealthBar() const
{
	if (InstancedWidget)
	{
		InstancedWidget->UpdateHPBar(GetHealth() / GetMaxHealth());
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

	if (DamageTags.HasTagExact(FGameplayTag::RequestGameplayTag("Ability.Action.Stealth")))
	{
		HideFloatingBar();
		return;
	}

	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DamageCauser->GetActorLocation()));

	PhysicalAnimComponent->HitReaction(HitInfo);
	if (!TargetCharacter)
	{
		TargetCharacter = InstigatorCharacter;
	}
}


void AEnemyBase::HighlightBorder() const
{
	ShowFloatingBar();
	InstancedWidget->HighlightBorder();
}

void AEnemyBase::UnHighlightBorder() const
{
	InstancedWidget->UnHighlightBorder();
}

void AEnemyBase::ShowFloatingBar() const
{
	InstancedWidget->SetVisibility(ESlateVisibility::Visible);
}

void AEnemyBase::HideFloatingBar() const
{
	InstancedWidget->SetVisibility(ESlateVisibility::Collapsed);
}
