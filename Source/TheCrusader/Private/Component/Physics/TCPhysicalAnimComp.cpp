﻿// Written by jaegang lim


#include "Component/Physics/TCPhysicalAnimComp.h"

#include "Character/TCGASCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Windows/Accessibility/WindowsUIAControlProvider.h"


UTCPhysicalAnimComp::UTCPhysicalAnimComp()
	: HitReactionTimeRemaining(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTCPhysicalAnimComp::HitReaction(const FHitResult& HitResult)
{
	SetComponentTickEnabled(true);
	HitReactionTimeRemaining += .9f;

	MeshComponent->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), true, false);
	ApplyPhysicalAnimationProfileBelow(TEXT("pelvis"), TEXT("Strong"), false, true);

	DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Emerald, false, 1.5f, 0, 5.f);
	FVector Trace = HitResult.TraceEnd - HitResult.TraceStart;
	UKismetMathLibrary::Vector_Normalize(Trace);
	Trace = Trace * 5000 * MeshComponent->GetMass();

	if (HitResult.BoneName.IsNone())
	{
		if (const ACharacter* Target = Cast<ACharacter>(HitResult.GetActor()))
		{
			const FName BoneName = Target->GetMesh()->FindClosestBone(HitResult.Location);
			MeshComponent->AddImpulse(Trace, BoneName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Something Wrong %s() in %s"), *FString(__FUNCTION__), *GetName());
		}
	}
	else
	{
		MeshComponent->AddImpulse(Trace, HitResult.BoneName == TEXT("pelvis") ? TEXT("spine_01") : HitResult.BoneName);
	}
}

void UTCPhysicalAnimComp::TogglePhyxsAnimation()
{
	// SetComponentTickEnabled(true);
	// HitReactionTimeRemaining += 2.f;
	//
	// ApplyPhysicalAnimationProfileBelow(TEXT("pelvis"), TEXT("Strong"), false, false);
	// MeshComponent->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), true, false);
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
