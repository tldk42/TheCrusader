// Written by jaegang lim


#include "Component/Mantle/MantleSystem.h"

#include "Character/TCGASCharacter.h"
#include "Character/Movement/TCMovementComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetMathLibrary.h"

UMantleSystem::UMantleSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMantleSystem::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ATCGASCharacter>(GetOwner());

	if (Character)
	{
		MovementComponent = Cast<UTCMovementComponent>(Character->GetMovementComponent());
		CapsuleComponent = Character->GetCapsuleComponent();
		AnimInstance = Character->GetMesh()->GetAnimInstance();

		// MantleTimeline = Character->GetWorld()->SpawnActor<AMantleTimeline>(TimelineClass)->Timeline;
	}
}

void UMantleSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MovementComponent)
	{
		if (MovementComponent->IsFalling() && Character->IsPawnControlled())
		{
			if (MovementComponent->GetCurrentAcceleration().Length() / MovementComponent->GetMaxAcceleration() > 0)
			{
				MantleFallingCheck();
			}
		}
	}
}

void UMantleSystem::ComponentWorldToLocal(FMantleComponentTransform& WorldSpace)
{
	WorldSpace.Transform = WorldSpace.Transform *
		UKismetMathLibrary::InvertTransform(WorldSpace.Component->GetComponentTransform());
}

void UMantleSystem::ComponentLocalToWorld(FMantleComponentTransform& LocalSpace)
{
	const FTransform InvertedTransform = UKismetMathLibrary::InvertTransform(
		LocalSpace.Component->GetComponentTransform());

	LocalSpace.Transform.SetLocation(
		UKismetMathLibrary::InverseTransformLocation(InvertedTransform, LocalSpace.Transform.GetLocation()));
	LocalSpace.Transform.SetRotation(
		UKismetMathLibrary::InverseTransformRotation(InvertedTransform, LocalSpace.Transform.GetRotation().Rotator()).
		Quaternion());
	LocalSpace.Transform.SetScale3D(
		UKismetMathLibrary::InverseTransformLocation(InvertedTransform, LocalSpace.Transform.GetScale3D()));
}

FTransform UMantleSystem::TransformToTransform(const FTransform& A, const FTransform& B)
{
	const FTransform ResultTransform
	(
		A.Rotator() - B.Rotator(),
		A.GetTranslation() - B.GetTranslation(),
		A.GetScale3D() - B.GetScale3D()
	);

	return ResultTransform;
}

bool UMantleSystem::MantleCheck_Implementation(FMantleTraceSettings TraceSettings) const
{
	return false;
}

bool UMantleSystem::MantleGroundCheck_Implementation() const
{
	return MantleCheck(GroundTraceSettings);
}

bool UMantleSystem::MantleFallingCheck_Implementation() const
{
	return MantleCheck(FallingTraceSettings);
}

void UMantleSystem::MantleStart_Implementation(const float Height, FMantleComponentTransform MantleLedgeWS,
                                               const EMantleType MantleType)
{
	// Step 1: Get the Mantle Asset and use it to set the new Mantle Params.
	const FMantleAsset MantleAsset = GetMantleAsset(MantleType);

	FMantleParams TempMantleParams;

	TempMantleParams.AnimMontage = MantleAsset.AnimMontage;
	TempMantleParams.CorrectionCurve = MantleAsset.CorrectionCurveVector;
	TempMantleParams.StartingOffset = MantleAsset.StartingOffset;
	TempMantleParams.StartingPosition = UKismetMathLibrary::MapRangeClamped(Height, MantleAsset.LowHeight,
	                                                                    MantleAsset.HighHeight,
	                                                                    MantleAsset.LowStartPosition,
	                                                                    MantleAsset.HighStartPosition);
	TempMantleParams.PlayRate = UKismetMathLibrary::MapRangeClamped(Height, MantleAsset.LowHeight,
	                                                            MantleAsset.HighHeight,
	                                                            MantleAsset.LowPlayRate,
	                                                            MantleAsset.HighPlayRate);

	// Step 2: Convert the world space target to the mantle component's local space for use in moving objects.
	ComponentWorldToLocal(MantleLedgeWS);

	// Step 3: Set the Mantle Target and calculate the Starting Offset (offset amount between the actor and target transform).
	MantleTarget = MantleLedgeWS.Transform;
	MantleActualStartOffset = TransformToTransform(Character->GetActorTransform(), MantleTarget);

	// Step 4: Calculate the Animated Start Offset from the Target Location.
	// This would be the location the actual animation starts at relative to the Target Transform.
	FVector V = UKismetMathLibrary::Quat_GetAxisX(MantleTarget.Rotator().Quaternion())
		* TempMantleParams.StartingOffset.Y;
	V.Z = TempMantleParams.StartingOffset.Z;

	const FTransform ATransform(
		MantleTarget.GetRotation(),
		MantleTarget.GetLocation() - V,
		FVector::One());
	MantleAnimatedStartOffset = TransformToTransform(ATransform, MantleTarget);

	// Step 5: Clear the Character Movement Mode and set the Movement State to Mantling
	MovementComponent->SetMovementMode(MOVE_None);

	// Step 6: Configure the Mantle Timeline so that it is the same length as the Lerp/Correction curve minus the starting position,
	// and plays at the same speed as the animation. Then start the timeline.
	float MinTime;
	float MaxTime;
	TempMantleParams.CorrectionCurve->GetTimeRange(MinTime, MaxTime);
	MantleTimeline->SetTimelineLength(MaxTime - TempMantleParams.StartingPosition);
	MantleTimeline->SetPlayRate(TempMantleParams.PlayRate);
	MantleTimeline->PlayFromStart();

	// Step 7: Play the Anim Montaget if valid.
	if (IsValid(TempMantleParams.AnimMontage))
	{
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(TempMantleParams.AnimMontage, TempMantleParams.PlayRate,
			                           EMontagePlayReturnType::MontageLength, TempMantleParams.StartingPosition);
		}
	}
}

void UMantleSystem::MantleUpdate_Implementation(const float BlendIn)
{
	// Step 1: Continually update the mantle target from the stored local transform to follow along with moving objects.
	ComponentLocalToWorld(MantleLedgeLS);
	MantleTarget = MantleLedgeLS.Transform;

	// Step 2: Update the Position and Correction Alphas using the Position/Correction curve set for each Mantle.

	const FVector TempV = MantleParams.CorrectionCurve->GetVectorValue(
		MantleTimeline->GetPlaybackPosition() + MantleParams.StartingPosition);

	const float PositionAlpha = TempV.X;
	const float XYCorrectionAlpha = TempV.Y;
	const float ZCorrectionAlpha = TempV.Z;

	// Step 3: Lerp multiple transforms together for independent control over the horizontal and vertical blend to the animated start position,
	// as well as the target position.

	// Blend into the animated horizontal and rotation offset using the Y value of the Position/Correction Curve.
	const FTransform YTransform(
		MantleAnimatedStartOffset.Rotator(),
		FVector(MantleAnimatedStartOffset.GetLocation().X, MantleAnimatedStartOffset.GetLocation().Y,
		        MantleActualStartOffset.GetLocation().Z),
		FVector::OneVector);

	UKismetMathLibrary::TLerp(MantleActualStartOffset, YTransform, XYCorrectionAlpha);

	// Blend into the animated vertical offset using the Z value of the Position/Correction Curve.
	const FTransform ZTransform(
		MantleActualStartOffset.Rotator(),
		FVector(MantleActualStartOffset.GetLocation().X, MantleActualStartOffset.GetLocation().Y,
		        MantleAnimatedStartOffset.GetLocation().Z),
		FVector::OneVector);

	UKismetMathLibrary::TLerp(MantleActualStartOffset, ZTransform, ZCorrectionAlpha);

	const FTransform Out = TransformToTransform(MantleTarget, FTransform(YTransform.GetRotation(),
	                                                                     FVector(YTransform.GetLocation().X,
	                                                                             YTransform.GetLocation().Y,
	                                                                             ZTransform.GetLocation().Z),
	                                                                     FVector::OneVector));

	// Blend from the currently blending transforms into the final mantle target using the X value of the Position/Correction Curve.
	UKismetMathLibrary::TLerp(Out, MantleTarget, PositionAlpha);

	// Initial Blend In (controlled in the timeline curve) to allow the actor to blend into the Position/Correction curve at the midoint.
	// This prevents pops when mantling an object lower than the animated mantle.
	FTransform LerpedTarget = UKismetMathLibrary::TLerp(TransformToTransform(MantleTarget, MantleActualStartOffset),
	                                                    UKismetMathLibrary::TLerp(Out, MantleTarget, PositionAlpha),
	                                                    BlendIn);

	// Step 4: Set the actors location and rotation to the Lerped Target.
	Character->SetActorLocationAndRotation(
		FVector(LerpedTarget.GetLocation().X, LerpedTarget.GetLocation().Y,
		        LerpedTarget.GetLocation().Z + MantleZOffset),
		LerpedTarget.GetRotation()
	);
}

void UMantleSystem::MantleEnd_Implementation()
{
	MovementComponent->SetMovementMode(MOVE_Walking);
}

bool UMantleSystem::CapsuleHasRoomCheck_Implementation(FVector TargetLocation, float HeightOffset, float RadiusOffset,
                                                       EDrawDebugTrace::Type DebugType)
{
	return false;
}

FMantleAsset UMantleSystem::GetMantleAsset_Implementation(const EMantleType MantleType) const
{
	switch (MantleType)
	{
	case EMantleType::Low:
		return HandType == EHandType::RightHand ? Mantle1MRH : Mantle1MLH;
	case EMantleType::Mid:
	case EMantleType::High:
	case EMantleType::FallingCatch:
		return Mantle2M;
	default:
		return Mantle1MLH;
	}
}
