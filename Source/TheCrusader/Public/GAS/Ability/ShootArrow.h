// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCGameplayAbility.h"
#include "ShootArrow.generated.h"

class ATCProjectile;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UShootArrow : public UTCGameplayAbility
{
	GENERATED_BODY()

public:
	UShootArrow();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* IdleShootMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* AimShootMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ATCProjectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Range;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Damage;

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};
