// Written by jaegang lim


#include "Character/EnemyBase.h"

#include "Component/Physics/TCPhysicalAnimComp.h"
#include "Components/WidgetComponent.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "GAS/Attribute/TCAttributeSet.h"
#include "UI/FloatingBar/EnemyBar.h"
#include "UI/FloatingBar/NPCBarBase.h"


AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	FloatingWidget = CreateDefaultSubobject<UWidgetComponent>("FloatingWidget");
	FloatingWidget->SetupAttachment(GetMesh(), FName("root"));
	FloatingWidget->SetRelativeLocation(FVector(0, 0, 190.f));
	FloatingWidget->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingWidget->SetDrawAtDesiredSize(true);


	AbilitySystemComponent = CreateDefaultSubobject<UTCAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSetBase = CreateDefaultSubobject<UTCAttributeSet>("AttributeSet");
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

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

void AEnemyBase::OnDamaged(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags,
                           ATCGASCharacter* InstigatorCharacter, AActor* DamageCauser)
{
	Super::OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}


void AEnemyBase::HighlightBorder() const
{
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
