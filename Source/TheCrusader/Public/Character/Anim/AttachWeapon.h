// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AttachWeapon.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API UAttachWeapon : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
