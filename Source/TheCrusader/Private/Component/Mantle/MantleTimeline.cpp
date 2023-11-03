// Written by jaegang lim


#include "Component/Mantle/MantleTimeline.h"

AMantleTimeline::AMantleTimeline()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMantleTimeline::BeginPlay()
{
	Super::BeginPlay();

	// MantleSystem = GetOwner()->GetComponentByClass<UMantleSystem>();
}