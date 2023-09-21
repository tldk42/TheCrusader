// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "LockAllAbility.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class THECRUSADER_API ULockAllAbility : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                         float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> GameplayEffect;
};
