// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Library/TCInterpolator.h"
#include "UObject/Object.h"
#include "TCCameraMode.generated.h"

class ATCPlayerController;
class ATCPlayerCameraManager;
class UCineCameraComponent;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UTCCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UTCCameraMode();

	virtual void SkipNextInterpolation();

	virtual void OnBecomeActive(AActor* ViewTarget, UTCCameraMode* PreviouslyActiveMode);

	virtual void OnBecomeInactive(AActor* ViewTarget, UTCCameraMode* NewActiveMode);

	virtual void UpdateCamera(AActor* ViewTarget, UCineCameraComponent* CineCamComp, float DeltaTime,
	                          FTViewTarget& OutVT);

	virtual void OnRemovedFromStack()
	{
	};

	float GetTransitionTime() const;

	UPROPERTY(Transient)
	ATCPlayerCameraManager* PlayerCameraManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition", meta = (DisplayName = "Transition In Time"))
	float TransitionInTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	FViewTargetTransitionParams TransitionParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings",
		meta = (EditCondition = "!bUseCineCamSettings"))
	float FOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam")
	bool bUseCineCam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam")
	bool bUseCineCamSettings;

	UPROPERTY(Transient)
	TArray<AActor*> BlockingActors;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ResetToDefaultSettings();

	virtual bool ShouldLockOutgoingPOV() const;

protected:
	bool bIsActive;

	/** Camera shake class to create when this camera mode becomes active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	/** Camera Shake object instance created when this camera mode is activated and a valid CameraShakeClass is set */
	UPROPERTY(transient)
	UCameraShakeBase* CameraShakeInstance = nullptr;

	/** When true, camera shake will be scaled using the ShakeScaling_SpeedRange and the ShakeScaling_ScaleRange */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
	bool bScaleShakeWithViewTargetVelocity;

	/** Ranges of view target velocity that will be mapped to a camera shake scale */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake",
		meta = (EditCondition = bScaleShakeWithViewTargetVelocity))
	FVector2D ShakeScaling_SpeedRange{0.f, 300.f};

	/** Maps to ShakeScaling_SpeedRange, such that at SpeedRange.X, shake will scale to ScaleRange.X */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake",
		meta = (EditCondition = bScaleShakeWithViewTargetVelocity))
	FVector2D ShakeScaling_ScaleRange{1.f, 3.f};

	/** Interpolator for smooth changes in camera shake intensities */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake",
		meta = (EditCondition = bScaleShakeWithViewTargetVelocity))
	FIIRInterpolatorFloat ShakeScaleInterpolator = FIIRInterpolatorFloat(5.f);

	/** When true, the camera mode will attempt to display debug information related to camera shake*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
	bool bDrawDebugShake;

	/** When UseCustomFocusDistance is true, this event should be implemented to determine a focus distance appropriate for the camera mode */
	UFUNCTION(BlueprintImplementableEvent)
	float GetCustomFocusDistance(AActor* ViewTarget, const FTransform& ViewToWorld) const;

	/** When true, a custom focus distance will be provided to the cine cam via an implementation of the GetCustomFocusDistance Blueprint Implementable Event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam", meta = (EditCondition = bUseCineCamSettings))
	bool bUseCustomFocusDistance;

	/** When true, custom view pitch limits will be used for this camera mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Manager Overrides")
	bool bOverrideViewPitchMinAndMax;

	/** Pitch minimum limit to use when OverrideViewPitchMinAndMax is true */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Manager Overrides",
		meta = (EditCondition = bOverrideViewPitchMinAndMax))
	float ViewPitchMinOverride;

	/** Pitch maximum limit to use when OverrideViewPitchMinAndMax is true */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Manager Overrides",
		meta = (EditCondition = bOverrideViewPitchMinAndMax))
	float ViewPitchMaxOverride;

	/** Attempts to cast and return the camera modes's owning CitySample Player Controller */
	ATCPlayerController* GetOwningTCPC() const;

	/** Attempts to determine the camera mode's desired focus distance, whether it be calculated normally or if a custom focus distance method is used  */
	virtual float GetDesiredFocusDistance(AActor* ViewTarget, const FTransform& ViewToWorld) const;

	/** When true, camera modes will reset certain interpolators. Useful for hard cuts or unique camera situations */
	bool bSkipNextInterpolation;

	/** Camera to world transform that is cached and can be used when something wants to easily access the camera's transform */
	FTransform LastCameraToWorld;
};
