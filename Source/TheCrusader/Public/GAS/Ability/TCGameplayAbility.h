// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCAbilityTypes.h"
#include "Abilities/GameplayAbility.h"
#include "TCGameplayAbility.generated.h"

struct FTCGameplayEffectContainerSpec;
struct FTCGameplayEffectContainer;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UTCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTCGameplayAbility()
	{
	}

	// 태그와 그와 관련된 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffects)
	TMap<FGameplayTag, FTCGameplayEffectContainer> EffectContainerMap;

	/**
	 * @param Container 어떤 대상에게 어떤 효과를 주고싶은지 (원본)
	 * @return 최종적으로 게임에서 사용할 수 있는 형태의 Spec을 반환
	 */
	UFUNCTION(BlueprintCallable, Category = Ability, meta=(AutoCreateRefTerm = "EventData"))
	virtual FTCGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(
		const FTCGameplayEffectContainer& Container,
		const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** Search for and make a gameplay effect container spec to be applied later, from the EffectContainerMap */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FTCGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag,
	                                                               const FGameplayEventData& EventData,
	                                                               int32 OverrideGameplayLevel = -1);

	// 효과적용
	/** Applies a gameplay effect container spec that was previously created */
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(
		const FTCGameplayEffectContainerSpec& ContainerSpec);

	/** Applies a gameplay effect container, by creating and then applying the spec */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag ContainerTag,
	                                                                 const FGameplayEventData& EventData,
	                                                                 int32 OverrideGameplayLevel = -1);

	//
};
