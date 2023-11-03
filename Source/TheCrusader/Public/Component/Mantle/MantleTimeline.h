// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MantleTimeline.generated.h"

// class UTimelineComponent;
// class UMantleSystem;

UCLASS()
class THECRUSADER_API AMantleTimeline : public AActor
{
	GENERATED_BODY()

public:
	AMantleTimeline();

protected:
	virtual void BeginPlay() override;

public:
	// UPROPERTY(BlueprintReadWrite)
	// UMantleSystem* MantleSystem;
	//
	// UPROPERTY(BlueprintReadWrite)
	// UTimelineComponent* Timeline;
};
