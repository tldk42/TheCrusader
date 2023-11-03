// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MantleSystem.generated.h"

class AMantleTimeline;
class UCurveVector;
class UTimelineComponent;
class UCapsuleComponent;
class UTCMovementComponent;
class ATCGASCharacter;

UENUM(BlueprintType)
enum class EMantleType : uint8
{
	Low,
	Mid,
	High,
	FallingCatch
};

UENUM(BlueprintType)
enum class EHandType : uint8
{
	LeftHand,
	RightHand
};

USTRUCT(BlueprintType)
struct FMantleComponentTransform
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FTransform Transform;

	UPROPERTY(EditAnywhere)
	UPrimitiveComponent* Component;
};

USTRUCT(BlueprintType)
struct FMantleAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere)
	UCurveVector* CorrectionCurveVector;

	UPROPERTY(EditAnywhere)
	FVector StartingOffset;

	UPROPERTY(EditAnywhere)
	float LowHeight;
	UPROPERTY(EditAnywhere)
	float LowPlayRate;
	UPROPERTY(EditAnywhere)
	float LowStartPosition;
	UPROPERTY(EditAnywhere)
	float HighHeight;
	UPROPERTY(EditAnywhere)
	float HighPlayRate;
	UPROPERTY(EditAnywhere)
	float HighStartPosition;
};

USTRUCT(BlueprintType)
struct FMantleTraceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MaxLedgeHeight;

	UPROPERTY(EditAnywhere)
	float MinLedgeHeight;

	UPROPERTY(EditAnywhere)
	float ReachDistance;

	UPROPERTY(EditAnywhere)
	float ForwardTraceRadius;

	UPROPERTY(EditAnywhere)
	float DownwardTraceRadius;
};

USTRUCT(BlueprintType)
struct FMantleParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere)
	UCurveVector* CorrectionCurve;

	UPROPERTY(EditAnywhere)
	FVector StartingOffset;

	UPROPERTY(EditAnywhere)
	float StartingPosition;

	UPROPERTY(EditAnywhere)
	float PlayRate;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THECRUSADER_API UMantleSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UMantleSystem();

protected:
#pragma region Actor Component

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion Actor Component

	UFUNCTION(BlueprintNativeEvent, Category = MantleSystem)
	bool CapsuleHasRoomCheck(FVector TargetLocation, float HeightOffset, float RadiusOffset,
	                         EDrawDebugTrace::Type DebugType);
	UFUNCTION(BlueprintNativeEvent, Category = MantleSystem)
	FMantleAsset GetMantleAsset(EMantleType MantleType) const;

	UFUNCTION(BlueprintNativeEvent, Category = MantleSystem)
	void MantleStart(float Height, FMantleComponentTransform MantleLedgeWS, EMantleType MantleType);
	UFUNCTION(BlueprintNativeEvent, Category = MantleSystem)
	void MantleUpdate(float BlendIn);
	UFUNCTION(BlueprintNativeEvent, Category = MantleSystem)
	void MantleEnd();
	UFUNCTION(BlueprintNativeEvent, Category = MantleSystem)
	bool MantleCheck(FMantleTraceSettings TraceSettings) const;
	UFUNCTION(BlueprintNativeEvent, Category = MantleSystem)
	bool MantleGroundCheck() const;
	UFUNCTION(BlueprintNativeEvent, Category = MantleSystem)
	bool MantleFallingCheck() const;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetHandType(const EHandType NewHandType) { HandType = NewHandType; }

	// UFUNCTION(BlueprintCallable)
	// FORCEINLINE void SetAnimType (const EAnimType NewAnimType) {}

private:
	static void ComponentWorldToLocal(FMantleComponentTransform& WorldSpace);
	static void ComponentLocalToWorld(FMantleComponentTransform& LocalSpace);
	static FTransform TransformToTransform(const FTransform& A, const FTransform& B);

private:
	UPROPERTY()
	ATCGASCharacter* Character;

	UPROPERTY()
	UTCMovementComponent* MovementComponent;

	UPROPERTY()
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY()
	UTimelineComponent* MantleTimeline;

	UPROPERTY()
	TSubclassOf<AMantleTimeline> TimelineClass;

	UPROPERTY()
	EHandType HandType;

	UPROPERTY(EditAnywhere, Category = AnimAsset)
	FMantleAsset Mantle2M;

	UPROPERTY(EditAnywhere, Category = AnimAsset)
	FMantleAsset Mantle1MRH;

	UPROPERTY(EditAnywhere, Category = AnimAsset)
	FMantleAsset Mantle1MLH;

	UPROPERTY(EditAnywhere, Category = MantleSystem)
	FMantleComponentTransform MantleLedgeLS;

	UPROPERTY(EditAnywhere, Category = MantleSystem)
	FMantleParams MantleParams;

	UPROPERTY(EditAnywhere, Category = MantleSystem)
	FTransform MantleAnimatedStartOffset;

	UPROPERTY(EditAnywhere, Category = MantleSystem)
	FTransform MantleActualStartOffset;

	UPROPERTY(EditAnywhere, Category = MantleSystem)
	FTransform MantleTarget;

	UPROPERTY(EditAnywhere, Category = Configs)
	FMantleTraceSettings FallingTraceSettings;
	UPROPERTY(EditAnywhere, Category = Configs)
	FMantleTraceSettings GroundTraceSettings;
	UPROPERTY(EditAnywhere, Category = Configs)
	float MantleZOffset;
};
