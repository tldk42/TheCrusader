// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/TCCameraMode.h"
#include "TCPlayerCameraManager.generated.h"

UENUM()
enum class ECameraStyle : uint8
{
	None,
	Fixed,
	ThirdPerson,
	FreeCam,
	FirstPerson
};

USTRUCT()
struct FActiveTCCamera
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UTCCameraMode* CameraMode = nullptr;

	UPROPERTY()
	AActor* ViewTarget = nullptr;

	float TransitionAlpha = 0.f;

	float TransitionUpdateRate = 0.f;

	float BlendWeight = 0.f;

	int32 InstanceIndex = INDEX_NONE;

	FMinimalViewInfo LastPOV;

	bool bLockOutgoingPOV = false;
};

USTRUCT(BlueprintType)
struct FActiveTCCameraModeInstance
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TSubclassOf<UTCCameraMode> CameraModeClass;

	UPROPERTY()
	AActor* ViewTarget = nullptr;

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly)
	UTCCameraMode* CameraMode = nullptr;

	void UpdateCamera(float DeltaTime, FTViewTarget& OutVT);
};


class ABalian;
/**
 * 
 */
UCLASS()
class THECRUSADER_API ATCPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ATCPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

#pragma region PlayerCameraManager Interface
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
#pragma endregion PlayerCameraManager Interface

	UFUNCTION(BlueprintCallable)
	UTCCameraMode* GetCurrentCameraMode();

	UFUNCTION(BlueprintCallable)
	int32 GetBestCameraMode(AActor* InViewTarget);

	FMinimalViewInfo GetTransitionGoalPOV() const
	{
		return TransitionGoalPOV;
	}

	UFUNCTION(BlueprintCallable, Category = Camera)
	void SetDebugCameraStyle(ECameraStyle NewDebugCameraStyle);

	UFUNCTION(BlueprintCallable)
	void SetViewPitchLimits(float MinPitch, float MaxPitch);

	UFUNCTION(BlueprintCallable)
	void ResetViewPitchLimits();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BasePelvisRelativeZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PelvisBoneName;

	UFUNCTION(BlueprintCallable)
	void ConfigureAlternateCamera(TSubclassOf<UTCCameraMode> NewAltCameraMode, AActor* NewAltViewTarget,
	                              float NewAltCameraTransitionTime);

	UFUNCTION(BlueprintCallable)
	void SetUsingAlternateCamera(bool bNewUsingAltCamera);

	UFUNCTION(BlueprintCallable)
	void ClearAlternateCamera();

	UFUNCTION(BlueprintCallable)
	bool IsUsingAlternateCamera() const
	{
		return bUsingAltCameraMode;
	}

	UFUNCTION(BlueprintCallable)
	bool HasAlternateCameraAvailable() const
	{
		return AltCameraMode != nullptr;
	}

protected:

	UFUNCTION(BlueprintNativeEvent, Category = Camera)
	TSubclassOf<UTCCameraMode> GetCameraClassForCharacter(const AActor* InViewTarget) const;
	TSubclassOf<UTCCameraMode> GetCameraClassForCharacter_Implementation(const AActor* InViewTarget) const;
	
	void TransitionToAltCamera();

	void TransitionFromAltCamera();

	void UpdateCameraInStack(int32 StackIdx, float DeltaTime, FTViewTarget& OutVT);

	float GetModeTransitionTime(const UTCCameraMode* ToMode) const;

	ATCPlayerController* GetOwningTCPC() const;

private:
	TSubclassOf<UTCCameraMode> DetermineBestCameraClass(AActor const* InViewTarget) const;

	int32 FindOrCreateCameraModeInstance(TSubclassOf<UTCCameraMode> CameraModeClass, AActor* InViewTarget);

	void CleanUpOutDatedCameraModeInstances();

public:
	bool bUsingAltCameraMode;

	TSubclassOf<UTCCameraMode> AltCameraMode;

	UPROPERTY(Transient)
	AActor* AltViewTarget;

	float AltCameraTransitionTime;

	UPROPERTY(Transient)
	AActor* SavedMainViewTarget;

protected:
	UPROPERTY(Transient)
	TArray<FActiveTCCamera> CameraBlendStack;

	UPROPERTY(Transient, EditInstanceOnly, BlueprintReadOnly)
	TArray<FActiveTCCameraModeInstance> CameraModeInstances;

private:
	FMinimalViewInfo TransitionGoalPOV;

	float DefaultMinPitchLimit = 0.f;

	float DefaultMaxPitchLimit = 0.f;
};
