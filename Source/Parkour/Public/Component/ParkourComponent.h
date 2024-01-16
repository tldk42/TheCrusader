// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ParkourComponent.generated.h"


struct FGameplayTag;
class UCameraComponent;
class UMotionWarpingComponent;
class UCapsuleComponent;
class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PARKOUR_API UParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UParkourComponent();

public:
	static float ClimbStyleValues(const FGameplayTag& ClimbStyleTag, const float Braced, const float FreeHang);

	/* 파쿠르에 필요한 변수 초기화(캐릭터, 무브먼트, 애니메이션, 메시, 모션워핑...) */
	void InitializeReference(UMotionWarpingComponent* InMotionWarpingComponent);

	/* 파쿠르 액션 발동(모든 파쿠르는 여기서 시작) */
	UFUNCTION(BlueprintCallable)
	void TriggerParkourAction(const bool bJumpAction);

protected:
	virtual void BeginPlay() override;

	/// 앞에 벽이 있는지 판별
	/// @return 벽에 대한 HitResult
	FHitResult DetectWall() const;

	/* 캡슐, 무브먼트를 지정한 방식으로 설정 */
	void ParkourStateSettings(const ECollisionEnabled::Type CollisionType, const EMovementMode NewMovementMode,
	                          const bool bStopMovementImmediately) const;

	/* 파쿠르 상태를 지정한 방식으로 설정 (SetParkourState 인터페이스 실행) */
	void SetParkourState(const FGameplayTag& NewStateTag);
	/* 벽을 오를 때 스타일을 지정한 방식으로 설정 (SetClimbStyle 인터페이스 실행) */
	void SetClimbStyle(const FGameplayTag& NewClimbStyleTag);
	/* 벽을 오르는 방향을 지정한 방향으로 설정 (SetClimbDirection 인터페이스 실행) */
	void SetClimbDirection(const FGameplayTag& NewClimbDirectionTag);

private:
#pragma region Helper
	static FVector VectorMoveUp(const FVector& InVector, const float Value);
	static FVector VectorMoveDown(const FVector& InVector, const float Value);
	static FVector VectorMoveLeft(const FVector& InVector, const float Value, const FRotator& Rotator);
	static FVector VectorMoveRight(const FVector& InVector, const float Value, const FRotator& Rotator);
	static FVector VectorMoveForward(const FVector& InVector, const float Value, const FRotator& Rotator);
	static FVector VectorMoveBackward(const FVector& InVector, const float Value, const FRotator& Rotator);
	static FRotator ReverseNormal(const FVector& InVector);
	static FRotator AddRotator(const FRotator& InRotator, const FRotator& AddRotator);
	void DrawDebug(const bool bDraw, const FVector& Location, const float DebugRadius, const FColor& DebugColor,
	               const float DebugDuration) const;

#pragma endregion

	void GridScanForHitResults(const int ScanBreadth, const int ScanHeight, const FVector& ScanBaseLocation,
	                           const FRotator& ScanRotation);
	/* 식별된 벽이 존재하면 벽의 높이, 깊이, 거리를 지정 */
	void MeasureWall();
	/* ParkourStateTag에 따라서 파쿠르타입을 지정 */
	void DecideParkourType(const bool bJumpAction) const;
	/* 루트(하단 중심)에서 박스 트레이스를 실행, 땅에 붙어있는지 판별 (bOnGround를 설정) */
	void ValidateOnGround();
	/* 식별된 벽에 대한 정보를 모두 초기화 */
	void ResetParkourResults();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY()
	ACharacter* CharacterRef;

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY()
	USkeletalMeshComponent* CharacterMeshComponent;

	UPROPERTY()
	UCapsuleComponent* CharacterCapsuleComponent;

	UPROPERTY()
	UAnimInstance* CharacterAnimInstance;

	UPROPERTY()
	UMotionWarpingComponent* CharacterMotionWarpingComponent;

protected:
	UPROPERTY()
	FGameplayTag ParkourStateTag = FGameplayTag::RequestGameplayTag("Parkour.State.FreeRoam");

	UPROPERTY()
	FGameplayTag ParkourActionTag = FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction");

	UPROPERTY()
	FGameplayTag ClimbStyleTag;

	UPROPERTY()
	FGameplayTag ClimbDirectionTag;

	UPROPERTY()
	FHitResult WallHitResult;

	UPROPERTY()
	FHitResult WallTopResult;

	UPROPERTY()
	FHitResult WallDepthResult;

	UPROPERTY()
	FHitResult WallVaultResult;

	UPROPERTY()
	FRotator WallRotation;

protected:
	UPROPERTY()
	float WallHeight;

	UPROPERTY()
	float WallDepth;

	UPROPERTY()
	float VaultHeight;

	UPROPERTY()
	bool bOnGround;
};
