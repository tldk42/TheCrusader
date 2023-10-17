// Written by jaegang lim


#include "GAS/Ability/TCGameplayAbility.h"

#include "Character/TCGASCharacter.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "GAS/Ability/TCTargetType.h"

FTCGameplayEffectContainerSpec UTCGameplayAbility::MakeEffectContainerSpecFromContainer(
	const FTCGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FTCGameplayEffectContainerSpec ReturnSpec;
	AActor* OwningActor = GetOwningActorFromActorInfo();
	ATCGASCharacter* OwningCharacter = Cast<ATCGASCharacter>(GetAvatarActorFromActorInfo());
	UTCAbilitySystemComponent* OwningASC = UTCAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);

	if (OwningASC)
	{
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			const UTCTargetType* TargetTypeCD0 = Container.TargetType.GetDefaultObject();
			AActor* AvatarActor = GetAvatarActorFromActorInfo();
			TargetTypeCD0->GetTargets(OwningCharacter, AvatarActor, EventData, HitResults, TargetActors);
			ReturnSpec.AddTargets(HitResults, TargetActors);
		}

		if (OverrideGameplayLevel == INDEX_NONE)
		{
			OverrideGameplayLevel = OverrideGameplayLevel = this->GetAbilityLevel();
		}

		for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
		{
			ReturnSpec.TargetGameplayEffectSpecs.
			           Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
		}
	}

	return ReturnSpec;
}

FTCGameplayEffectContainerSpec UTCGameplayAbility::MakeEffectContainerSpec(FGameplayTag ContainerTag,
                                                                           const FGameplayEventData& EventData,
                                                                           int32 OverrideGameplayLevel)
{
	if (const FTCGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag))
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FTCGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> UTCGameplayAbility::ApplyEffectContainerSpec(
	const FTCGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of effect specs and apply them to their target data
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> UTCGameplayAbility::ApplyEffectContainer(FGameplayTag ContainerTag,
                                                                             const FGameplayEventData& EventData,
                                                                             int32 OverrideGameplayLevel)
{
	const FTCGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
	return ApplyEffectContainerSpec(Spec);
}
