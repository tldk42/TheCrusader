// Written by jaegang lim


#include "Component/ParkourComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/ParkourAnimation.h"
#include "MotionWarpingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#pragma region Helper
FVector UParkourComponent::VectorMoveUp(const FVector& InVector, const float Value)
{
	return InVector + FVector(0.f, 0.f, Value);
}

FVector UParkourComponent::VectorMoveDown(const FVector& InVector, const float Value)
{
	return InVector - FVector(0.f, 0.f, Value);
}

FVector UParkourComponent::VectorMoveLeft(const FVector& InVector, const float Value, const FRotator& Rotator)
{
	return InVector + Value * Rotator.Quaternion().GetRightVector() * -1;
}

FVector UParkourComponent::VectorMoveRight(const FVector& InVector, const float Value, const FRotator& Rotator)
{
	return InVector + Value * Rotator.Quaternion().GetRightVector();
}

FVector UParkourComponent::VectorMoveForward(const FVector& InVector, const float Value, const FRotator& Rotator)
{
	return InVector + Value * Rotator.Quaternion().GetForwardVector();
}

FVector UParkourComponent::VectorMoveBackward(const FVector& InVector, const float Value, const FRotator& Rotator)
{
	return InVector + (Value * Rotator.Quaternion().GetForwardVector() * -1);
}

FRotator UParkourComponent::ReverseNormal(const FVector& InVector)
{
	return FRotator(
		0, UKismetMathLibrary::NormalizedDeltaRotator(UKismetMathLibrary::MakeRotFromX(InVector),
		                                              FRotator(0, 180.f, 0)).Yaw, 0);
}

FRotator UParkourComponent::AddRotator(const FRotator& InRotator, const FRotator& AddRotator)
{
	return FRotator(InRotator.Pitch + AddRotator.Pitch, InRotator.Yaw + AddRotator.Yaw,
	                InRotator.Roll + AddRotator.Roll);
}

void UParkourComponent::DrawDebug(const bool bDraw, const FVector& Location, const float DebugRadius,
                                  const FColor& DebugColor, const float DebugDuration) const
{
	if (bDraw)
	{
		DrawDebugSphere(GetWorld(), Location, DebugRadius, 12, DebugColor, false, DebugDuration);
	}
}
#pragma endregion

UParkourComponent::UParkourComponent()
	: CharacterRef(nullptr),
	  CharacterMovementComponent(nullptr),
	  CharacterMeshComponent(nullptr),
	  CharacterCapsuleComponent(nullptr),
	  CharacterAnimInstance(nullptr),
	  CharacterMotionWarpingComponent(nullptr),
	  WallHeight(0),
	  WallDepth(0),
	  VaultHeight(0),
	  bOnGround(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UParkourComponent::InitializeReference(UMotionWarpingComponent* InMotionWarpingComponent)
{
	CharacterRef = Cast<ACharacter>(GetOwner());
	check(CharacterRef)

	CharacterMovementComponent = CharacterRef->GetCharacterMovement();

	CharacterMeshComponent = CharacterRef->GetMesh();
	check(CharacterMeshComponent)

	CharacterCapsuleComponent = CharacterRef->GetCapsuleComponent();

	CharacterAnimInstance = CharacterMeshComponent->GetAnimInstance();

	CharacterMotionWarpingComponent = InMotionWarpingComponent;
}

float UParkourComponent::ClimbStyleValues(const FGameplayTag& ClimbStyleTag, const float Braced, const float FreeHang)
{
	if (ClimbStyleTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Parkour.Style.BracedClimb")))
	{
		return Braced;
	}
	if (ClimbStyleTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Parkour.Style.FreeHand")))
	{
		return FreeHang;
	}
	return 0;
}


void UParkourComponent::GridScanForHitResults(const int ScanBreadth, const int ScanHeight,
                                              const FVector& ScanBaseLocation, const FRotator& ScanRotation)
{
	TArray<FHitResult> WallHitResults;
	TArray<FHitResult> LineHitResults;

	// 왼쪽 ----> 오른쪽
	for (int i = 0; i <= ScanBreadth; ++i)
	{
		LineHitResults.Empty();

		// 아래쪽 ----> 위쪽
		for (int j = 0; j <= ScanHeight; ++j)
		{
			FHitResult HitResult;
			FVector StartLocation = VectorMoveBackward(
				VectorMoveUp(VectorMoveRight(ScanBaseLocation, (i * 20 - ScanBreadth * 10), ScanRotation), j * 8), 60.f,
				ScanRotation);
			FVector EndLocation = VectorMoveForward(
				VectorMoveUp(VectorMoveRight(ScanBaseLocation, (i * 20 - ScanBreadth * 10), ScanRotation), j * 8), 60.f,
				ScanRotation);
			GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility);

			// 무언가 앞에 존재한다면 LineTrace결과에 추가
			LineHitResults.Add(HitResult);
		}

		float CachedDistance = -99.f; // 저장된 거리
		int Index = 0;
		// 앞에 존재하는 개체(Trace 결과)들에 대해서 Iteration을 시도
		/// --------------------> 
		/// --------------------> 이 지점을 WallHitResult에 넣어준다. (이 지점은 플레이어가 손으로 짚어야할 후보가 될것) 
		/// 플레이어 ---- |
		/// 플레이어 ---- |
		for (const FHitResult& LineHitResult : LineHitResults)
		{
			float Distance = LineHitResult.bBlockingHit
				                 ? LineHitResult.Distance
				                 : FVector::Distance(LineHitResult.TraceStart, LineHitResult.TraceEnd);

			// 이전 Trace결과와 거리 차이가 난다는것은 벽 위에 어느 지점이라는 의미
			if (CachedDistance != -99.f && (Distance - CachedDistance) > 5.f)
			{
				WallHitResults.Add(LineHitResults[Index - 1]);
				break;
			}

			CachedDistance = Distance;
			Index++;
		}
	}

	// 잡아야 할 벽의 위치를 찾지 못한게 아니라면 (너무 높거나 없거나)
	if (!WallHitResults.IsEmpty())
	{
		// 후보들 중에서 플레이어로 부터 가장 가까운 점을 찾는다.
		// * * * <*> * * * * *
		//        +           
		//      player        
		for (int i = 0; i < WallHitResults.Num(); ++i)
		{
			if (i == 0)
			{
				WallHitResult = WallHitResults[i];
				continue;
			}

			float CurrentWallHitDistance = FVector::Distance(WallHitResults[i].ImpactPoint,
			                                                 CharacterRef->GetActorLocation());
			float PreviousWallHitDistance = FVector::Distance(WallHitResult.ImpactPoint,
			                                                  CharacterRef->GetActorLocation());

			if (CurrentWallHitDistance < PreviousWallHitDistance)
			{
				WallHitResult = WallHitResults[i];
			}
		}
		DrawDebug(true, WallHitResult.ImpactPoint, 2.5, FColor::Green, 2.f);

		// 이제 최적의 위치로 부터 이 장애물을 넘을 수 있을지 판별해야한다.
		if (WallHitResult.bBlockingHit && !WallHitResult.bStartPenetrating)
		{
			FHitResult CachedTopHit;

			if (ParkourStateTag != FGameplayTag::RequestGameplayTag("Parkour.State.Climb"))
			{
				WallRotation = ReverseNormal(WallHitResult.ImpactNormal);
			}

			// 플레이어의 앞쪽으로 다시 SphereTrace를 시작
			///			
			///			 *   - - - - - - - - (한번에 넘어갈 수 있을지??)
			/// player   |  ---------------
			for (int i = 0; i <= 8; ++i)
			{
				FVector SphereTraceStart = VectorMoveUp(
					VectorMoveForward(WallHitResult.ImpactPoint, i * 30, WallRotation), 7.f);
				FVector SphereTraceEnd = VectorMoveDown(SphereTraceStart, 7.f);

				FHitResult HitResult;
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(CharacterRef);
				UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, 2.5f,
				                                        TraceTypeQuery1, false, ActorsToIgnore,
				                                        EDrawDebugTrace::ForOneFrame, HitResult, true);

				if (i == 0 && HitResult.bBlockingHit)
				{
					WallTopResult = HitResult;
					DrawDebug(true, HitResult.ImpactPoint, 5.f, FColor::Yellow, 4.f);
				}
				// 계속해서 뭔가가 찾아지는 중(장애물)
				else if (HitResult.bBlockingHit)
				{
					CachedTopHit = HitResult;
					DrawDebug(true, HitResult.ImpactPoint, 5.f, FColor::Magenta, 4.f);
				}
				// 걸리는게 없다 (즉, 넘어갈 수 있는 장애물인 것으로 판별)
				else
				{
					if (ParkourStateTag == FGameplayTag::RequestGameplayTag("Parkour.State.FreeRoam"))
					{
						HitResult.Reset();
						UKismetSystemLibrary::SphereTraceSingle(
							GetWorld(), VectorMoveForward(CachedTopHit.ImpactPoint, 30.f, WallRotation),
							CachedTopHit.ImpactPoint, 10.f, TraceTypeQuery1, false, ActorsToIgnore,
							EDrawDebugTrace::ForOneFrame, HitResult, true);

						if (HitResult.bBlockingHit)
						{
							WallDepthResult = HitResult;
							DrawDebug(true, WallDepthResult.ImpactPoint, 10.f, FColor::Blue, 5.f);

							HitResult.Reset();
							FVector TraceStart = VectorMoveForward(WallDepthResult.ImpactPoint, 70.f, WallRotation);
							FVector TraceEnd = VectorMoveDown(TraceStart, 200.f);
							UKismetSystemLibrary::SphereTraceSingle(
								GetWorld(), TraceStart, TraceEnd, 10.f, TraceTypeQuery1, false, ActorsToIgnore,
								EDrawDebugTrace::ForOneFrame, HitResult, true);

							if (HitResult.bBlockingHit)
							{
								WallVaultResult = HitResult;
								DrawDebug(true, WallVaultResult.ImpactPoint, 5.f, FColor::Purple, 5.f);
							}
						}
					}
					break;
				}
			}
		}
	}
}

void UParkourComponent::MeasureWall()
{
	if (WallHitResult.bBlockingHit && WallTopResult.bBlockingHit)
	{
		WallHeight = WallTopResult.ImpactPoint.Z - CharacterMeshComponent->GetSocketLocation("root").Z;

		if (WallDepthResult.bBlockingHit)
		{
			WallDepth = FVector::Distance(WallTopResult.ImpactPoint, WallDepthResult.ImpactPoint);
		}
		else
		{
			WallDepth = 0.f;
		}

		if (WallDepthResult.bBlockingHit && WallVaultResult.bBlockingHit)
		{
			VaultHeight = WallDepthResult.ImpactPoint.Z - WallVaultResult.ImpactPoint.Z;
		}
		else
		{
			VaultHeight = 0.f;
		}
	}
	else
	{
		WallHeight = 0.f;
		WallDepth = 0.f;
		VaultHeight = 0.f;
	}
}

void UParkourComponent::DecideParkourType(const bool bJumpAction) const
{
	if (WallTopResult.bBlockingHit)
	{
		if (ParkourStateTag == FGameplayTag::RequestGameplayTag("Parkour.State.FreeRoam"))
		{
			if (bOnGround)
			{
				if (WallHeight >= 90.f && WallHeight <= 160.f)
				{
					if (WallDepth >= 0.f && WallDepth <= 120.f)
					{
						if (VaultHeight >= 60.f && VaultHeight <= 120.f)
						{
							if (CharacterMovementComponent->Velocity.Length() > 20.f)
							{
								UE_LOG(LogTemp, Display, TEXT("Try To Vault"));
							}
							else
							{
								UE_LOG(LogTemp, Display, TEXT("Try To Mantle"));
							}
						}
						else
						{
							UE_LOG(LogTemp, Display, TEXT("Try To Mantle"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Display, TEXT("Try To Mantle"));
					}
				}
				else
				{
					if (WallHeight < 250.f)
					{
						UE_LOG(LogTemp, Display, TEXT("Try To Climb"));
					}
					else
					{
						UE_LOG(LogTemp, Display, TEXT("No Action"));
					}
				}
			}
		}
		else if (ParkourStateTag == FGameplayTag::RequestGameplayTag("Parkour.State.Climb"))
		{
			UE_LOG(LogTemp, Display, TEXT("Climb Type"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("No Parkour Action"));

		if (bJumpAction)
		{
			CharacterRef->Jump();
		}
	}
}

void UParkourComponent::ValidateOnGround()
{
	if (ParkourStateTag != FGameplayTag::RequestGameplayTag("Parkour.State.Climb"))
	{
		FHitResult HitResult;
		const FVector TraceStart = CharacterMeshComponent->GetSocketLocation("root");
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(CharacterRef);

		UKismetSystemLibrary::BoxTraceSingle(GetWorld(), TraceStart, TraceStart, FVector(10, 10, 4),
		                                     FRotator::ZeroRotator, TraceTypeQuery1, false, ActorsToIgnore,
		                                     EDrawDebugTrace::ForOneFrame, HitResult,
		                                     true);

		bOnGround = HitResult.bBlockingHit;
	}
	else
	{
		bOnGround = false;
	}
}

void UParkourComponent::ResetParkourResults()
{
	WallHitResult.Reset();
	WallTopResult.Reset();
	WallDepthResult.Reset();
	WallVaultResult.Reset();

	WallHeight = 0.f;
	WallDepth = 0.f;
	VaultHeight = 0.f;
}

void UParkourComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ValidateOnGround();

	if (bOnGround)
	{
		if (ParkourActionTag == FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"))
		{
			ResetParkourResults();
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("In Air"));

		if (ParkourStateTag == FGameplayTag::RequestGameplayTag("Parkour.State.FreeRoam"))
		{
			TriggerParkourAction(false);
		}
		else
		{
		}
	}
}


void UParkourComponent::TriggerParkourAction(const bool bJumpAction)
{
	// 아무런 파쿠르 액션을 진행중이 아님
	if (ParkourActionTag == FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"))
	{
		const FHitResult HitResult = DetectWall();

		// 앞에 벽이 존재? (물론 이 벽의 높이는 플레이어가 도달 할 수 있는 벽)
		if (HitResult.bBlockingHit)
		{
			// 벽에 대해서 Grid형태로 LineTrace를 진행하며 최적의 위치를 찾는다.
			GridScanForHitResults(4, 30, HitResult.ImpactPoint, ReverseNormal(HitResult.ImpactNormal));
			// 벽에 대한 정보 기입
			MeasureWall();
			// 파쿠르 타입을 결정
			DecideParkourType(bJumpAction);
		}
		else if (bJumpAction)
		{
			// 앞에 상호작용 가능한 물체가 존재하지 않으면 점프를 시도
			CharacterRef->Jump();
		}
	}
}

FHitResult UParkourComponent::DetectWall() const
{
	// 공중에 있으면 Iteration을 절반만 수행
	int LastIndex = CharacterMovementComponent->IsFalling() ? 9 : 16;

	// 플레이어의 위치에서부터 위로 일정한 간격으로 SphereTrace를 실행하고 가장 먼저 맞춰진 결과를 반환한다. (앞쪽으로 발사)
	for (int i = 0; i < LastIndex; ++i)
	{
		FVector CharacterBaseLocation = VectorMoveUp(VectorMoveDown(CharacterRef->GetActorLocation(), 60), i * 16.f);

		FVector TraceStart = VectorMoveBackward(CharacterBaseLocation, 20.f, CharacterRef->GetActorRotation());
		FVector TraceEnd = VectorMoveForward(CharacterBaseLocation, 140.f, CharacterRef->GetActorRotation());

		FHitResult TraceResult;
		TArray<AActor*> IgnoreActor;
		IgnoreActor.Add(CharacterRef);
		UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, 10.f, TraceTypeQuery1, false,
		                                        IgnoreActor, EDrawDebugTrace::None, TraceResult, true);


		if (TraceResult.bBlockingHit && !TraceResult.bStartPenetrating)
		{
			DrawDebug(false, TraceResult.ImpactPoint, 5.f, FColor::Cyan, 1.f);
			return TraceResult;
		}
	}
	return FHitResult();
}

void UParkourComponent::ParkourStateSettings(const ECollisionEnabled::Type CollisionType,
                                             const EMovementMode NewMovementMode,
                                             const bool bStopMovementImmediately) const
{
	CharacterCapsuleComponent->SetCollisionEnabled(CollisionType);
	CharacterMovementComponent->SetMovementMode(NewMovementMode);

	if (bStopMovementImmediately)
		CharacterMovementComponent->StopMovementImmediately();
}

void UParkourComponent::SetParkourState(const FGameplayTag& NewStateTag)
{
	if (NewStateTag != ParkourStateTag)
	{
		ParkourStateTag = NewStateTag;

		IParkourAnimation::Execute_SetParkourState(CharacterAnimInstance, ParkourStateTag);

		if (ParkourStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Parkour.State.FreeRoam")))
		{
			ParkourStateSettings(ECollisionEnabled::QueryAndPhysics, EMovementMode::MOVE_Walking, false);
		}
		else if (ParkourStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Parkour.State.ReadyToClimb")))
		{
			ParkourStateSettings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, false);
		}
		else if (ParkourStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Parkour.State.Climb")))
		{
			ParkourStateSettings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, true);
		}
		else if (ParkourStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Parkour.State.Mantle")))
		{
			ParkourStateSettings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, false);
		}
		else if (ParkourStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Parkour.State.Vault")))
		{
			ParkourStateSettings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, false);
		}
	}
}

void UParkourComponent::SetClimbStyle(const FGameplayTag& NewClimbStyleTag)
{
	if (ClimbStyleTag != NewClimbStyleTag)
	{
		ClimbStyleTag = NewClimbStyleTag;

		IParkourAnimation::Execute_SetClimbStyle(CharacterAnimInstance, ClimbStyleTag);
	}
}

void UParkourComponent::SetClimbDirection(const FGameplayTag& NewClimbDirectionTag)
{
	if (ClimbDirectionTag != NewClimbDirectionTag)
	{
		ClimbDirectionTag = NewClimbDirectionTag;

		IParkourAnimation::Execute_SetClimbDirection(CharacterAnimInstance, ClimbDirectionTag);
	}
}
