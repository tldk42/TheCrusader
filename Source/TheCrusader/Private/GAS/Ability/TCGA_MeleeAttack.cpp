// Written by jaegang lim


#include "GAS/Ability/TCGA_MeleeAttack.h"

#include "Character/TCGASCharacter.h"
#include "GAS/TCAT_PlayMontageAndWaitForEvent.h"

UTCGA_MeleeAttack::UTCGA_MeleeAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag("Ability.Action.LightAttack");
	AbilityTags.AddTag(AttackTag);
	ActivationOwnedTags.AddTag(AttackTag);

	Damage = FMath::RandRange(10, 20);
}

void UTCGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	UTCAT_PlayMontageAndWaitForEvent* Task = UTCAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this,NAME_Name, AttackMontage, FGameplayTagContainer(), 1.f);
	Task->OnBlendOut.AddDynamic(this, &ThisClass::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &ThisClass::OnCancelled);
	Task->EventReceived.AddDynamic(this, &ThisClass::EventReceived);
	Task->ReadyForActivation();
}

void UTCGA_MeleeAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UTCGA_MeleeAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UTCGA_MeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
	// {
	// 	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	// 	return;
	// }

	// if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.Attack")))
	// {
	// 	ATCGASCharacter* EventOwner = Cast<ATCGASCharacter>(GetAvatarActorFromActorInfo());
	// 	if (!EventOwner)
	// 	{
	// 		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	// 	}
	//
	// 	
	// }

}
