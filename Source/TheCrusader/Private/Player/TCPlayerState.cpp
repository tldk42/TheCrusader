// Written by jaegang lim


#include "Player/TCPlayerState.h"
#include "Quest/QuestManager.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "GAS/Attribute/TCAttributeSet.h"

ATCPlayerState::ATCPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UTCAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UTCAttributeSet>(TEXT("AttributeSetBase"));

	QuestManager = CreateDefaultSubobject<UQuestManager>(TEXT("QuestManager"));

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* ATCPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UTCAttributeSet* ATCPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

float ATCPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float ATCPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float ATCPlayerState::GetHealthRegenRate() const
{
	return AttributeSetBase->GetHealthRegenRate();
}

float ATCPlayerState::GetStamina() const
{
	return AttributeSetBase->GetStamina();
}

float ATCPlayerState::GetMaxStamina() const
{
	return AttributeSetBase->GetMaxStamina();
}

float ATCPlayerState::GetStaminaRegenRate() const
{
	return AttributeSetBase->GetStaminaRegenRate();
}

void ATCPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->
		                              GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).
		                              AddUObject(this, &ThisClass::HealthChanged);

		// Tag change callbacks
		// AbilitySystemComponent->RegisterGameplayTagEvent(KnockedDownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::KnockDownTagChanged);
	}
}

void ATCPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	// ABalian* Player = Cast<ABalian>(GetPawn());
	// // Player 유효성 && DeadTag상태인지 확인
	// if (IsValid(Player) && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	// {
	// 	if (!AbilitySystemComponent->HasMatchingGameplayTag(KnockedDownTag))
	// 	{
	// 		// Player->KnockDown();
	// 	}
	// 	else
	// 	{
	// 		// Player->FinishDying();
	// 	}
	// }
}
