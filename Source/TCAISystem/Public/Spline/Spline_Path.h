// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spline_Path.generated.h"

class USplineComponent;

UCLASS()
class TCAISYSTEM_API ASpline_Path : public AActor
{
	GENERATED_BODY()

public:
	ASpline_Path();

	UFUNCTION(BlueprintCallable)
	void IncrementPatrolRoute();

	UFUNCTION(BlueprintCallable)
	FVector GetSplinePointAsWorldPosition() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent* SplineComponent;

	UPROPERTY(BlueprintReadWrite)
	int SplineIndex;

	UPROPERTY(BlueprintReadWrite)
	int Direction;
};
