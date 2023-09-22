// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TCAT_PlayMontageAndWaitForEvent.generated.h"

class UTCAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTCPlayMontageAndWaitForEventDelegate, FGameplayTag, EventTag,
                                             FGameplayEventData, EventData);


/**
 *  PlayMontageAndWait & WaitForEvent를 조합한 Task
 */
UCLASS()
class THECRUSADER_API UTCAT_PlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()

public:
#pragma region Constructor&Overrides
	UTCAT_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
#pragma endregion Constructor&Overrides

#pragma region Delegates
	
	/** Complete */
	UPROPERTY(BlueprintAssignable)
	FTCPlayMontageAndWaitForEventDelegate OnCompleted;

	/** Blending Out */
	UPROPERTY(BlueprintAssignable)
	FTCPlayMontageAndWaitForEventDelegate OnBlendOut;

	/** Interrupted */
	UPROPERTY(BlueprintAssignable)
	FTCPlayMontageAndWaitForEventDelegate OnInterrupted;

	/** Cancelled */
	UPROPERTY(BlueprintAssignable)
	FTCPlayMontageAndWaitForEventDelegate OnCancelled;

	/** Event Received */
	UPROPERTY(BlueprintAssignable)
	FTCPlayMontageAndWaitForEventDelegate EventReceived;

#pragma endregion Delegates

	/**
	 * 몽타주를 재생하고 끝나기까지 기다린다.
	 * 특정이벤트 태그와 일치하는 이벤트(또는 비어있는)가 발생하게 되면,
	 * EventReceived 델리게이트가 관련된 함수들을 실행시킨다.
	 * If StopWhenAbilityEnds is true, this montage will be aborted if the ability ends normally.
	 * It is always stopped when the ability is explicitly cancelled.
	 * On normal execution, OnBlendOut is called when the montage is blending out, and OnCompleted when it is completely done playing
	 * OnInterrupted is called if another montage overwrites this, and OnCancelled is called if the ability or task is cancelled
	 *
	 * @param OwningAbility
	 * @param TaskInstanceName Set to override the name of this task, for later querying
	 * @param MontageToPlay The montage to play on the character
	 * @param EventTags Any gameplay events matching this tag will activate the EventReceived callback. If empty, all events will trigger callback
	 * @param Rate Change to play the montage faster or slower
	 * @param StartSection
	 * @param bStopWhenAbilityEnds If true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled
	 * @param AnimRootMotionTranslationScale Change to modify size of root motion or set to 0 to block it entirely
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UTCAT_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		UAnimMontage* MontageToPlay,
		FGameplayTagContainer EventTags,
		float Rate = 1.f,
		FName StartSection = NAME_None,
		bool bStopWhenAbilityEnds = true,
		float AnimRootMotionTranslationScale = 1.f);

private:

#pragma region MontageProperties
	
	/** 재생할 몽타주 */
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	/** 이벤트 태그 컨테이너 */
	UPROPERTY()
	FGameplayTagContainer EventTags;

	/** 몽타주 속도 */
	UPROPERTY()
	float Rate;

	/** 몽타주 시작 섹션 */
	UPROPERTY()
	FName StartSection;

	/** 루트모션 강도 */
	UPROPERTY()
	float AnimRootMotionTranslationScale;

	/** 어빌리티 끝나면 몽타주 끄기 */
	UPROPERTY()
	bool bStopWhenAbilityEnds;

#pragma endregion MontageProperties

	
	/** 몽타주를 정지시킨다. */
	bool StopPlayingMontage();

	/** ASC 반환 */
	UTCAbilitySystemComponent* GetTargetASC();

	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
};
