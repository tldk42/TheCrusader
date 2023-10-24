// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TCMovementComponent.generated.h"

class ATCCharacterBase;

USTRUCT(BlueprintType)
struct FTCGroundInfo
{
	GENERATED_BODY()

	FTCGroundInfo()
		: LastUpdateFrame(0),
		  GroundDistance(0)
	{
	}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THECRUSADER_API UTCMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	float TimeLastSlideAlongSurfaceBlock = -1000.0f;

public:
	UTCMovementComponent();

	/** Ground 정보를 반환 */
	UFUNCTION(BlueprintCallable, Category = "EssentialData")
	const FTCGroundInfo& GetGroundInfo();

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void StartAiming();
	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void StopAiming();

#pragma region UCharacterMovementComponent Interface
	virtual void PhysicsRotation(float DeltaTime) override;
	virtual float GetMaxSpeed() const override;

protected:
	// 이동 상태에 따라서 가속도를 보간한다.
	virtual FVector ConstrainInputAcceleration(const FVector& InputAcceleration) const override;
	// SlideAlongSurface 는 이동하다가 충돌 발생시 그냥 제자리에 멈춰 벽이나 경사로에 "달라붙기" 보다는, 그 표면을 타고 부드럽게 미끄러지도록 하는 데 관련된 물리 계산 처리를 합니다.
	virtual float SlideAlongSurface(const FVector& Delta, float Time, const FVector& InNormal, FHitResult& Hit,
	                                bool bHandleImpact) override;

#pragma endregion UCharacterMovementComponent Interface

public:
	UFUNCTION(BlueprintCallable)
	bool WasSlideAlongSurfaceBlockedRecently(float Tolerance) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
	float ADSSpeedMultiplier = 0.7f;
	uint8 RequestToStartADS : 1;

protected:
	// ConstraintInputAcceleration will interpolate the input size to go from this value to 1.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinInputAccelerationSize = 0.0f;

	// Minimum angle for movement to slide against. This prevents sliding against very sharp angles that will result in very small movements. Only active if greater than 0.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinHorizontalSurfaceSlideAngle = 35.f;

	// Minimum angle for movement to slide against when colliding against a character
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinHorizontalSurfaceSlideAngleCharacter = 20.f;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool bLastSurfaceWasCharacter = false;

	// Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
	FTCGroundInfo CachedGroundInfo;
};
