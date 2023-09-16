// Written by jaegang lim


#include "GAS/TCAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GAS/Ability/TCGameplayAbility.h"


UTCAbilitySystemComponent::UTCAbilitySystemComponent()
{
}


void UTCAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer,
                                                           TArray<UTCGameplayAbility*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActive;
	// 태그와 관련된 어빌리티 모두 반환
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActive, false);

	// 관련된 어빌리티에서 현재 활성화 된게 있는지 확인할 것
	for (FGameplayAbilitySpec* Spec : AbilitiesToActive)
	{
		TArray<UGameplayAbility*> ActiveInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : ActiveInstances)
		{
			ActiveAbilities.Add(Cast<UTCGameplayAbility>(ActiveAbility));
		}
	}
}

void UTCAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UTCAbilitySystemComponent::GetShouldTick() const
{
	return Super::GetShouldTick();
}

void UTCAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}

void UTCAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
                                                   bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
}

UTCAbilitySystemComponent* UTCAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor,
	bool LookForComponent)
{
	return Cast<UTCAbilitySystemComponent>(
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}
