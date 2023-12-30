// Written by jaegang lim


#include "Spline/Spline_Path.h"

#include "Components/SplineComponent.h"


// Sets default values
ASpline_Path::ASpline_Path()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline Route");
	SplineComponent->SetupAttachment(GetRootComponent());
}

void ASpline_Path::IncrementPatrolRoute()
{
	SplineIndex += Direction;

	if (SplineIndex == SplineComponent->GetNumberOfSplinePoints() - 1)
	{
		Direction--;
	}
	else if (SplineIndex == 0)
	{
		Direction = 1;
	}
}

FVector ASpline_Path::GetSplinePointAsWorldPosition() const
{
	return SplineComponent->GetLocationAtSplinePoint(SplineIndex, ESplineCoordinateSpace::World);
}
