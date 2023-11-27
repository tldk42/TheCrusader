// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCCameraMode.h"
#include "TCCam_NPCDialogue.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class THECRUSADER_API UTCCam_NPCDialogue : public UTCCameraMode
{
	GENERATED_BODY()

	virtual void SkipNextInterpolation() override;
	virtual void OnBecomeActive(AActor* ViewTarget, UTCCameraMode* PreviouslyActiveMode) override;
	virtual void UpdateCamera(AActor* ViewTarget, UCineCameraComponent* CineCamComp, float DeltaTime, FTViewTarget& OutVT) override;
	
};
