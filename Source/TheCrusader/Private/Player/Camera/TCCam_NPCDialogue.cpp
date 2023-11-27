// Written by jaegang lim


#include "Player/Camera/TCCam_NPCDialogue.h"

#include "Player/TCPlayerCameraManager.h"

void UTCCam_NPCDialogue::SkipNextInterpolation()
{
	Super::SkipNextInterpolation();
}

void UTCCam_NPCDialogue::OnBecomeActive(AActor* ViewTarget, UTCCameraMode* PreviouslyActiveMode)
{
	Super::OnBecomeActive(ViewTarget, PreviouslyActiveMode);

	SkipNextInterpolation();
	LastCameraToWorld = PlayerCameraManager
		                    ? FTransform(PlayerCameraManager->GetCameraRotation(),
		                                 PlayerCameraManager->GetCameraLocation())
		                    : FTransform::Identity;
}

void UTCCam_NPCDialogue::UpdateCamera(AActor* ViewTarget, UCineCameraComponent* CineCamComp, float DeltaTime,
                                      FTViewTarget& OutVT)
{
	Super::UpdateCamera(ViewTarget, CineCamComp, DeltaTime, OutVT);

	UWorld const* const World = ViewTarget ? ViewTarget->GetWorld() : nullptr;
	if (World)
	{
		if (!IsValid(ViewTarget) || !PlayerCameraManager || !PlayerCameraManager->PCOwner)
		{
			return;
		}
	}
	PlayerCameraManager->ApplyCameraModifiers(DeltaTime, OutVT.POV);

	

	bSkipNextInterpolation = false;
}
