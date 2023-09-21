// Written by jaegang lim


#include "Character/Anim/NotifyState/LockAllAbility.h"

#include "AbilitySystemComponent.h"
#include "Character/Balian.h"

void ULockAllAbility::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ABalian* Player = Cast<ABalian>(MeshComp->GetOwner());
	if (Player)
	{
		Player->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(GameplayEffect.GetDefaultObject(), 1,
		                                                               Player->GetAbilitySystemComponent()->
		                                                                       MakeEffectContext());
	}
}

void ULockAllAbility::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABalian* Player = Cast<ABalian>(MeshComp->GetOwner());
	if (Player)
	{
		Player->GetAbilitySystemComponent()->RemoveActiveGameplayEffectBySourceEffect(
			GameplayEffect, Player->GetAbilitySystemComponent());
	}
}
