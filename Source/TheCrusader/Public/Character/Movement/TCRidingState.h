// Written by jaegang lim

#pragma once

#include "TCRidingState.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct THECRUSADER_API FTCRidingState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Throttle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Brake = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Steering = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHandbrakeOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ForwardSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RPM = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Gear = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomatic = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllWheelsOnGround = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNoWheelsOnGround = false;
};
