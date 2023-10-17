// Written by jaegang lim


#include "GAS/Ability/TCTargetType.h"
#include "Character/TCGASCharacter.h"
#include "Abilities/GameplayAbilityTypes.h"

void UTCTargetType::GetTargets_Implementation(ATCGASCharacter* TargetingCharacter, AActor* TargetingActor,
                                              FGameplayEventData EventData, TArray<FHitResult>& OutHitResults,
                                              TArray<AActor*>& OutActor) const
{
	return;
}

void UTargetType_UseOwner::GetTargets_Implementation(ATCGASCharacter* TargetingCharacter, AActor* TargetingActor,
                                                     FGameplayEventData EventData, TArray<FHitResult>& OutHitResults,
                                                     TArray<AActor*>& OutActor) const
{
	OutActor.Add(TargetingCharacter);
}

void UTargetType_UseEventData::GetTargets_Implementation(ATCGASCharacter* TargetingCharacter, AActor* TargetingActor,
                                                         FGameplayEventData EventData,
                                                         TArray<FHitResult>& OutHitResults,
                                                         TArray<AActor*>& OutActor) const
{
	if (const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult())
	{
		OutHitResults.Add(*FoundHitResult);
	}
	else if (EventData.TargetData.Num() > 0)
	{
		OutHitResults.Add(*EventData.TargetData.Get(0)->GetHitResult());
	}

	else if (EventData.Target)
	{
		OutActor.Add(const_cast<AActor*>(EventData.Target.Get()));
	}
}
