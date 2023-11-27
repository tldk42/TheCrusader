// Written by jaegang lim


#include "Item/Weapon/Item_Weapon_Bow.h"
#include "Components/CapsuleComponent.h"


AItem_Weapon_Bow::AItem_Weapon_Bow()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItem_Weapon_Bow::BeginPlay()
{
	Super::BeginPlay();
}

void AItem_Weapon_Bow::PredictTarget()
{
	// const FVector StartLocation = BowSkeletalMesh->GetSocketLocation("Muzzle");

	// const FVector LaunchVelocity = BowSkeletalMesh->GetSocketQuaternion("Muzzle").GetForwardVector() * 3800.f;
	//
	// FPredictProjectilePathResult PredictProjectilePathResult;
	// FPredictProjectilePathParams PredictProjectilePathParams
	// 	(-1.f, StartLocation, LaunchVelocity, 3.f, ECC_Visibility, GetParentActor());
	// PredictProjectilePathParams.bTraceWithCollision = true;
	// PredictProjectilePathParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	// PredictProjectilePathParams.TraceChannel = ECC_Visibility;
	// PredictProjectilePathParams.DrawDebugTime = 1.f;

	// if (UGameplayStatics::PredictProjectilePath(GetWorld(), PredictProjectilePathParams, PredictProjectilePathResult))
	// {
	// 	int PathNum = PredictProjectilePathResult.PathData.Num();;
	//
	// 	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	// 	ATC_HUD* PlayerHUD = Cast<ATC_HUD>(PlayerController->GetHUD());
	//
	// 	if (FVector2D PathToScreen; UGameplayStatics::ProjectWorldToScreen(PlayerController,
	// 	                                                                   PredictProjectilePathResult.PathData[
	// 		                                                                   FMath::Floor(PathNum * .5f)].
	// 	                                                                   Location, PathToScreen))
	// 	{
	// 		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / 2;
	// 		FVector2D FinalScreenPos = PathToScreen - ViewportSize;
	// 		// if (FinalScreenPos.SizeSquared() > 30000.f)
	// 		// {
	// 		// 	FinalScreenPos = FVector2D::ZeroVector;
	// 		// }
	// 		FMath::Clamp(FinalScreenPos.Y, -100.f, 100.f);
	// 		ScreenPos = FMath::Vector2DInterpTo(ScreenPos, FinalScreenPos, GetWorld()->GetDeltaSeconds(), 5.f);
	//
	// 		PlayerHUD->GetCrosshairWidget()->SetRenderTranslation(ScreenPos);
	// 	}
	// }
}
