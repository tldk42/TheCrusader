// Written by jaegang lim


#include "GAS/Attribute/TCAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Character/EnemyBase.h"
#include "Character/TCGASCharacter.h"
#include "Net/UnrealNetwork.h"

UTCAttributeSet::UTCAttributeSet()
{
	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Front"));
	HitDirectionBackTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Back"));
	HitDirectionRightTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Right"));
	HitDirectionLeftTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Left"));
}

void UTCAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// If a Max value changes, adjust current to keep Current % of Current to Max
	if (Attribute == GetMaxHealthAttribute())
	// GetMaxHealthAttribute comes from the Macros defined at the top of the header
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
	}
}

void UTCAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer& TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive)
	{
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ATCGASCharacter* TargetCharacter = nullptr;

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ATCGASCharacter>(TargetActor);
	}

	// Damage Effect
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		AActor* SourceActor = nullptr;
		AController* SourceController = nullptr;
		ATCGASCharacter* SourceCharacter = nullptr;

		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if (SourceController != nullptr && SourceActor != nullptr)
			{
				if (APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			if (SourceController)
			{
				SourceCharacter = Cast<ATCGASCharacter>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter = Cast<ATCGASCharacter>(SourceActor);
			}

			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}

		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		const float LocalDamageDone = GetDamage();
		SetDamage(.0f);

		if (LocalDamageDone > 0)
		{
			bool WasAlive = true;

			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->IsAlive();
			}

			if (!TargetCharacter->IsAlive())
			{
				// Logging....
			}

			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.f, GetMaxHealth()));

			if (TargetCharacter && WasAlive)
			{
				/**
				// const FHitResult* Hit = Data.EffectSpec.GetContext().GetHitResult();
				//
				// if (Hit)
				// {
				// 	ETCHitReactDirection HitDirection = TargetCharacter->GetHitReactDirection(
				// 		Data.EffectSpec.GetContext().GetHitResult()->Location);
				// 	switch (HitDirection)
				// 	{
				// 	case ETCHitReactDirection::None:
				// 	default:
				// 		TargetCharacter->PlayHitReact(HitDirectionFrontTag, SourceCharacter);
				// 		break;
				// 	case ETCHitReactDirection::Left:
				// 		TargetCharacter->PlayHitReact(HitDirectionLeftTag, SourceCharacter);
				// 		break;
				// 	case ETCHitReactDirection::Front:
				// 		TargetCharacter->PlayHitReact(HitDirectionFrontTag, SourceCharacter);
				// 		break;
				// 	case ETCHitReactDirection::Right:
				// 		TargetCharacter->PlayHitReact(HitDirectionRightTag, SourceCharacter);
				// 		break;
				// 	case ETCHitReactDirection::Back:
				// 		TargetCharacter->PlayHitReact(HitDirectionBackTag, SourceCharacter);
				// 		break;
				// 	}
				// }
				// else
				// {
				// 	TargetCharacter->PlayHitReact(HitDirectionFrontTag, SourceCharacter);
				// }
				**/

				TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor);

				TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
			}
		}
	}

	// Health Effect
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

		if (TargetCharacter)
		{
			TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
		}
	}

	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		if (TargetCharacter)
		{
			SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
			TargetCharacter->HandleStaminaChanged(DeltaValue, SourceTags);
		}
	}
}

void UTCAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UTCAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCAttributeSet, StaminaRegenRate, COND_None, REPNOTIFY_Always);


	DOREPLIFETIME_CONDITION_NOTIFY(UTCAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCAttributeSet, DefensePower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCAttributeSet, SkillPower, COND_None, REPNOTIFY_Always);
}

void UTCAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
                                                  const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
                                                  const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurrentMaxValue > 0.f)
			                       ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue
			                       : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UTCAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCAttributeSet, Health, OldHealth);
}

void UTCAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCAttributeSet, MaxHealth, OldMaxHealth);
}

void UTCAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UTCAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCAttributeSet, Stamina, OldStamina);
}

void UTCAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCAttributeSet, MaxStamina, OldMaxStamina);
}

void UTCAttributeSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCAttributeSet, StaminaRegenRate, OldStaminaRegenRate);
}

void UTCAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCAttributeSet, AttackPower, OldValue);
}

void UTCAttributeSet::OnRep_DefensePower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCAttributeSet, DefensePower, OldValue);
}

void UTCAttributeSet::OnRep_SkillPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCAttributeSet, SkillPower, OldValue);
}
