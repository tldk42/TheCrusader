// Written by jaegang lim


#include "Character/Movement/TCMovementComponent.h"

#include "Character/TCCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

namespace TheCrusader
{
	static float GroundTraceDistance = 100000.0f;
	FAutoConsoleVariableRef CVar_GroundTraceDistance(
		TEXT("TheCrusader.GroundTraceDistance"), GroundTraceDistance,
		TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
}

UTCMovementComponent::UTCMovementComponent()
{
}

const FTCGroundInfo& UTCMovementComponent::GetGroundInfo()
{
	// 전 프레임과 동일한 상태면 SKIP
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
		return CachedGroundInfo;

	// 땅위에서 걷고 있으면 HitResult 초기화
	if (MovementMode == MOVE_Walking)
	{
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.f;
	}
	else
	{
		const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		check(Capsule);

		// Line Trace (캐릭터 캡슐 중간 부터 -100000 까지) 
		const float CapsuleHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
		const ECollisionChannel CollisionChannel = (UpdatedComponent
			                                            ? UpdatedComponent->GetCollisionObjectType()
			                                            : ECC_Pawn);
		const FVector TraceStart(GetActorLocation());
		const FVector TraceEnd(TraceStart.X, TraceStart.Y,
		                       (TraceStart.Z - TheCrusader::GroundTraceDistance - CapsuleHalfHeight));

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(CharacterOwner);
		QueryParams.bTraceComplex = false;
		FCollisionResponseParams ResponseParams;
		InitCollisionParams(QueryParams, ResponseParams);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams,
		                                     ResponseParams);

		// 정보 업데이트
		CachedGroundInfo.GroundHitResult = HitResult;
		CachedGroundInfo.GroundDistance = TheCrusader::GroundTraceDistance;

		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = .0f;
		}
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), .0f);
		}
	}

	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	return CachedGroundInfo;
}

void UTCMovementComponent::PhysicsRotation(float DeltaTime)
{
	// const ATheCrusaderCharacter* const Character = Cast<ATheCrusaderCharacter>(CharacterOwner);
	//
	// if ()

	Super::PhysicsRotation(DeltaTime);
}

FVector UTCMovementComponent::ConstrainInputAcceleration(const FVector& InputAcceleration) const
{
	FVector Result = InputAcceleration;
	const float InputAccelerationSize = InputAcceleration.Size();
	UE_LOG(LogTemp, Warning, TEXT("%f"), InputAccelerationSize);
	if (InputAccelerationSize > SMALL_NUMBER)
	{
		const float ScaledInputAccelerationSize = FMath::Lerp(MinInputAccelerationSize, 1.f, InputAccelerationSize);
		Result = InputAcceleration * ScaledInputAccelerationSize / InputAccelerationSize;
	}

	Result = Super::ConstrainInputAcceleration(Result);
	return Result;
}

float UTCMovementComponent::SlideAlongSurface(const FVector& Delta, float Time, const FVector& InNormal,
                                              FHitResult& Hit, bool bHandleImpact)
{
	// Normal opposing movement, along which we will slide. (즉, 슬라이드 될 방향 / 표면의 법선)
	FVector Normal(InNormal);

	// 이 MovementComp와 부딪힌 개체가 Chacracter클래스인가?
	const bool bHitCharacter = Hit.GetHitObjectHandle().DoesRepresentClass(ACharacter::StaticClass());
	bLastSurfaceWasCharacter = bHitCharacter;
	const float EffectiveMinHorizontalSurfaceSlideAngle = bHitCharacter
		                                                      ? MinHorizontalSurfaceSlideAngleCharacter
		                                                      : MinHorizontalSurfaceSlideAngle;

	if (EffectiveMinHorizontalSurfaceSlideAngle > 0.f)
	{
		const FVector MovementDirection = Acceleration.GetSafeNormal2D();

		// 입력방향 | 슬라이딩 방향 내적 -> 두 벡터 사이의 각도
		const float MovementDotNormal = MovementDirection | Normal;
		if (FMath::Abs(MovementDotNormal) >
			FMath::Cos(FMath::DegreesToRadians(EffectiveMinHorizontalSurfaceSlideAngle)))
		{
			Normal = -MovementDirection;
			TimeLastSlideAlongSurfaceBlock = GetWorld()->GetTimeSeconds();
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

		const float DebugAngle = FMath::RadiansToDegrees(FMath::Acos(FMath::Abs(MovementDotNormal)));
		DrawDebugLine(GetWorld(), CharacterOwner->GetActorLocation(),
		              CharacterOwner->GetActorLocation() + Normal * 30.0f, FColor::Red, false, -1.0f,
		              SDPG_Foreground, 2.0f);
		DrawDebugLine(GetWorld(), CharacterOwner->GetActorLocation(),
		              CharacterOwner->GetActorLocation() + MovementDirection * 30.0f, FColor::Blue, false, -1.0f,
		              SDPG_Foreground, 2.0f);
		DrawDebugString(GetWorld(), CharacterOwner->GetActorLocation(), FString::SanitizeFloat(DebugAngle), nullptr,
		                FColor::White, 0.0f, false, 2.0f);

#endif
	}


	return Super::SlideAlongSurface(Delta, Time, Normal, Hit, bHandleImpact);
}

bool UTCMovementComponent::WasSlideAlongSurfaceBlockedRecently(float Tolerance) const
{
	if (const UWorld* const World = GetWorld())
	{
		const float TimeThreshold = FMath::Max(Tolerance, World->DeltaTimeSeconds + KINDA_SMALL_NUMBER);
		return World->TimeSince(TimeLastSlideAlongSurfaceBlock) <= TimeThreshold;
	}

	return false;
}
