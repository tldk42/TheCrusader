// Written by jaegang lim


#include "Character/TCGASCharacter.h"

#include "Components/SphereComponent.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "GAS/Ability/TCGameplayAbility.h"
#include "GAS/Attribute/TCAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ATCGASCharacter::ATCGASCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bAlwaysRelevant = true;
}

void ATCGASCharacter::JumpSectionForCombo()
{
	if (IsValid(JumpSectionNotify))
	{
		if (bEnableComboPeriod)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			const FName CurrentMontageName = AnimInstance->Montage_GetCurrentSection();
			const UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();

			const FName NextSectionName = JumpSectionNotify->JumpSections[UKismetMathLibrary::RandomInteger(
				JumpSectionNotify->JumpSections.Num())];

			AnimInstance->Montage_SetNextSection(CurrentMontageName, NextSectionName, CurrentMontage);

			bEnableComboPeriod = false;
		}
	}
}

float ATCGASCharacter::GetHealth() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float ATCGASCharacter::GetMaxHealth() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float ATCGASCharacter::GetStamina() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetStamina();
	}

	return 0.0f;
}

float ATCGASCharacter::GetMaxStamina() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetMaxStamina();
	}

	return 0.0f;
}

UAbilitySystemComponent* ATCGASCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ATCGASCharacter::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || !AbilitySystemComponent->
		bCharacterAbilitiesGiven)
	{
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterDefaultAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

void ATCGASCharacter::UpdateHealthBar() const
{
}

void ATCGASCharacter::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->
		bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UTCGameplayAbility>& StartupAbility : CharacterDefaultAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1,
			                     1, this));
	}

	for (const auto& MeleeAbility : MeleeAbilities)
	{
		FGameplayAbilitySpecHandle Spec = AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(
				MeleeAbility.Value, 1,
				1, this));
		MeleeAbilitySpec.Add(MeleeAbility.Key, Spec);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void ATCGASCharacter::InitializeAttributes()
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."),
		       *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
		DefaultAttributes, 1, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
			*NewHandle.Data.Get());
	}
}

void ATCGASCharacter::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->
		bStartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
			GameplayEffect, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
				*NewHandle.Data.Get(), AbilitySystemComponent);
		}
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}

void ATCGASCharacter::OnDamaged(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags,
                                ATCGASCharacter* InstigatorCharacter, AActor* DamageCauser)
{
}

void ATCGASCharacter::OnHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (GetHealth() == 0.f)
	{
		UE_LOG(LogTemp, Display, TEXT("DEAD"));
	}
}

void ATCGASCharacter::HandleDamage(float DamageAmount, const FHitResult& HitInfo,
                                   const FGameplayTagContainer& DamageTags, ATCGASCharacter* InstigatorCharacter,
                                   AActor* DamageCauser)
{
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}

void ATCGASCharacter::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	OnHealthChanged(DeltaValue, EventTags);
}
