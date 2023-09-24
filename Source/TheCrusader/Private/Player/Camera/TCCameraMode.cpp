// Written by jaegang lim


#include "Player/Camera/TCCameraMode.h"

#include "Player/TCPlayerCameraManager.h"
#include "Player/TCPlayerController.h"

UTCCameraMode::UTCCameraMode()
	: PlayerCameraManager(nullptr),
	  FOV(75.f),
	  TransitionInTime(0.5f),
	  bUseCineCam(false),
	  bUseCineCamSettings(false),
	  bIsActive(false),
	  bScaleShakeWithViewTargetVelocity(false),
	  bDrawDebugShake(false),
	  bUseCustomFocusDistance(false),
	  bOverrideViewPitchMinAndMax(false),
	  ViewPitchMinOverride(0.f),
	  ViewPitchMaxOverride(0.f),
	  bSkipNextInterpolation(false)
{
	TransitionParams.BlendFunction = VTBlend_Cubic;
}

ATCPlayerController* UTCCameraMode::GetOwningTCPC() const
{
	return PlayerCameraManager ? Cast<ATCPlayerController>(PlayerCameraManager->PCOwner) : nullptr;
}

void UTCCameraMode::SkipNextInterpolation()
{
	bSkipNextInterpolation = true;
	ShakeScaleInterpolator.Reset();
}

void UTCCameraMode::OnBecomeActive(AActor* ViewTarget, UTCCameraMode* PreviouslyActiveMode)
{
	if (CameraShakeClass != nullptr)
	{
		CameraShakeInstance = PlayerCameraManager->StartCameraShake(CameraShakeClass, 1.f);
	}

	if (PlayerCameraManager != nullptr)
	{
		if (bOverrideViewPitchMinAndMax)
		{
			PlayerCameraManager->SetViewPitchLimits(ViewPitchMinOverride, ViewPitchMaxOverride);
		}
		else
		{
			PlayerCameraManager->ResetViewPitchLimits();
		}
	}

	bIsActive = true;
}

void UTCCameraMode::OnBecomeInactive(AActor* ViewTarget, UTCCameraMode* NewActiveMode)
{
	// 비활성화 시 셰이킹 인스턴스 초기화, 플래그 초기화
	if (CameraShakeInstance)
	{
		PlayerCameraManager->StopCameraShake(CameraShakeInstance, false);
		CameraShakeInstance = nullptr;
	}

	bIsActive = false;
}

void UTCCameraMode::UpdateCamera(AActor* ViewTarget, UCineCameraComponent* CineCamComp, float DeltaTime,
                                 FTViewTarget& OutVT)
{
	if (CameraShakeInstance)
	{
		CameraShakeInstance->ShakeScale = 1.f;

		if (bScaleShakeWithViewTargetVelocity)
		{
			if (ViewTarget)
			{
				const float Speed = ViewTarget->GetVelocity().Size();
				const float GoalScale = FMath::GetMappedRangeValueClamped(
					ShakeScaling_SpeedRange, ShakeScaling_ScaleRange, Speed);
				CameraShakeInstance->ShakeScale = ShakeScaleInterpolator.Eval(GoalScale, DeltaTime);

				if (bDrawDebugShake)
				{
#if ENABLE_DRAW_DEBUG
					FlushDebugStrings(ViewTarget->GetWorld());
					DrawDebugString(ViewTarget->GetWorld(), ViewTarget->GetActorLocation() + FVector(0, 0, 60.f),
					                FString::Printf(TEXT("%f"), CameraShakeInstance->ShakeScale), nullptr,
					                FColor::Yellow);
#endif
				}
			}
		}
	}
}

float UTCCameraMode::GetTransitionTime() const
{
	return TransitionInTime;
}

void UTCCameraMode::ResetToDefaultSettings_Implementation()
{
	UTCCameraMode const* const ThisCDO = CastChecked<UTCCameraMode>(GetClass()->GetDefaultObject());

	FOV = ThisCDO->FOV;
	bUseCineCamSettings = ThisCDO->bUseCineCamSettings;
	bUseCineCam = ThisCDO->bUseCineCam;
	bUseCustomFocusDistance = ThisCDO->bUseCustomFocusDistance;
	TransitionInTime = ThisCDO->TransitionInTime;
	TransitionParams = ThisCDO->TransitionParams;
	ShakeScaling_SpeedRange = ThisCDO->ShakeScaling_SpeedRange;
	ShakeScaling_ScaleRange = ThisCDO->ShakeScaling_ScaleRange;
	ShakeScaleInterpolator = ThisCDO->ShakeScaleInterpolator;
}

bool UTCCameraMode::ShouldLockOutgoingPOV() const
{
	return TransitionParams.bLockOutgoing;
}


float UTCCameraMode::GetDesiredFocusDistance(AActor* ViewTarget, const FTransform& ViewToWorld) const
{
	if (bUseCustomFocusDistance)
	{
		const float Dist = GetCustomFocusDistance(ViewTarget, ViewToWorld);
		if (Dist > 0.f)
		{
			return Dist;
		}
	}

	FVector FocusPoint(0.f);
	if (ViewTarget)
	{
		FocusPoint = ViewTarget->GetActorLocation();
	}

	return (FocusPoint - ViewToWorld.GetLocation()).Size();
}
