﻿// Written by jaegang lim


#include "GAS/Attribute/TCDamageExecution.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Attribute/TCAttributeSet.h"
#include "AbilitySystemComponent.h"

struct TCDamageStatistic
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);

	TCDamageStatistic()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTCAttributeSet, DefensePower, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTCAttributeSet, AttackPower, Source, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTCAttributeSet, Damage, Source, true)
	}
};

static const TCDamageStatistic& DamageStatistic()
{
	static TCDamageStatistic DMGStatistic;
	return DMGStatistic;
}


UTCDamageExecution::UTCDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatistic().DefensePowerDef);
	RelevantAttributesToCapture.Add(DamageStatistic().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatistic().DamageDef);
}

void UTCDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();


	AActor* SourceActor = SourceAbilitySystemComponent
		                      ? SourceAbilitySystemComponent->GetAvatarActor_Direct()
		                      : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent
		                      ? TargetAbilitySystemComponent->GetAvatarActor_Direct()
		                      : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float DefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatistic().DefensePowerDef, EvaluationParameters,
	                                                           DefensePower);
	if (DefensePower == 0.0f)
	{
		DefensePower = 1.0f;
	}

	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatistic().AttackPowerDef, EvaluationParameters,
	                                                           AttackPower);

	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatistic().DamageDef, EvaluationParameters,
	                                                           Damage);
	Damage += FMath::Max<float>(
		Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.0f), 0.0f);
	Damage = FMath::FRandRange(Damage * 0.7, Damage * 1.3);
	if (const FHitResult* HitResult = ExecutionParams.GetOwningSpec().GetEffectContext().GetHitResult())
	{
		if (HitResult->BoneName == "head")
		{
			Damage *= 1.5f;
		}
	}
	const float DamageDone = Damage * AttackPower / DefensePower;

	if (TargetTags->HasTagExact(FGameplayTag::RequestGameplayTag("State.Parry")))
	{
		FGameplayEventData PayloadData;
		PayloadData.Target = SourceActor;
		PayloadData.Instigator = TargetActor;

		if (ExecutionParams.GetOwningSpec().GetEffectContext().GetHitResult())
		{
			PayloadData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(
				*ExecutionParams.GetOwningSpec().GetEffectContext().GetHitResult());
		}


		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor,
		                                                         FGameplayTag::RequestGameplayTag(
			                                                         "Ability.Action.Parry"), PayloadData);
	}
	else
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(DamageStatistic().DamageProperty, EGameplayModOp::Additive, DamageDone));
	}

	// if (DamageDone > 0.f)
	// {
	// }
	//
	// // Block중이라면 Attribute는 건들지 않고 큐만 진행
	// else if (TargetTags->HasTagExact(FGameplayTag::RequestGameplayTag("State.Blocking")))
	// {
	// 	if (TargetTags->HasTagExact(FGameplayTag::RequestGameplayTag("State.Parry")))
	// 	{
	// 		OutExecutionOutput.AddOutputModifier(
	// 			FGameplayModifierEvaluatedData(DamageStatistic().DamageProperty, EGameplayModOp::Additive, 0.f));
	// 	}
	// 	else
	// 	{
	// 		const FGameplayEventData PayloadData;
	// 		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor,
	// 		                                                         FGameplayTag::RequestGameplayTag(
	// 			                                                         "Event.Montage.BlockedHit"), PayloadData);
	// 	}
	// }
}
