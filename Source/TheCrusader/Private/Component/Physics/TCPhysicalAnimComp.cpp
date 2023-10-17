// Written by jaegang lim


#include "Component/Physics/TCPhysicalAnimComp.h"

#include "Character/TCGASCharacter.h"
#include "Kismet/KismetMathLibrary.h"


UTCPhysicalAnimComp::UTCPhysicalAnimComp()
	: HitReactionTimeRemaining(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTCPhysicalAnimComp::HitReaction(const FHitResult& HitResult)
{
	SetComponentTickEnabled(true);
	HitReactionTimeRemaining += 1;

	ApplyPhysicalAnimationProfileBelow(TEXT("pelvis"), TEXT("Strong"), false, false);
	MeshComponent->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), true, false);

	DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Red, false, 1.5f, 0, 5.f);
	FVector Trace = HitResult.TraceEnd - HitResult.TraceStart;
	UKismetMathLibrary::Vector_Normalize(Trace);
	Trace = Trace * 5000 * MeshComponent->GetMass();

	if (HitResult.BoneName == "None")
	{
		MeshComponent->AddImpulse(Trace, TEXT("head"), true);
	}
	else
	{
		MeshComponent->AddImpulse(Trace, HitResult.BoneName == TEXT("pelvis") ? TEXT("spine_01") : HitResult.BoneName,
		                          true);
	}
}

void UTCPhysicalAnimComp::TogglePhyxsAnimation()
{
}


void UTCPhysicalAnimComp::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);

	MeshComponent = Cast<ATCGASCharacter>(GetOwner())->GetMesh();

	SetSkeletalMeshComponent(MeshComponent);
}


void UTCPhysicalAnimComp::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HitReactionTimeRemaining =
		UKismetMathLibrary::FInterpTo(HitReactionTimeRemaining, 0, DeltaTime, 1) - (DeltaTime * 0.1f);

	if (HitReactionTimeRemaining <= 0.f)
	{
		HitReactionTimeRemaining = 0.f;
		MeshComponent->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), false, false);
		SetComponentTickEnabled(false);
	}
	else
	{
		{
			const float BlendWeight = HitReactionTimeRemaining <= 1 ? HitReactionTimeRemaining : 1;
			MeshComponent->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), HitReactionTimeRemaining, false, false);
		}
	}
}
