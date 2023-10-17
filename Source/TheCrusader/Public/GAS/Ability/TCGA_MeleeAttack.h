// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCGameplayAbility.h"
#include "TCGA_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API UTCGA_MeleeAttack : public UTCGameplayAbility
{
	GENERATED_BODY()
public:
	UTCGA_MeleeAttack();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* AttackMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Damage;

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};
