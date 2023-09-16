// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TCAbilitySystemComponent.generated.h"


class UTCGameplayAbility;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THECRUSADER_API UTCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UTCAbilitySystemComponent();

	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;


	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UTCGameplayAbility*>& ActiveAbilities);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool GetShouldTick() const override;
	
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;

	static UTCAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);

};
