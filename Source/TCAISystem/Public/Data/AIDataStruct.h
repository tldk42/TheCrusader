// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "AIDataStruct.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Patrol,
	Investigating,
	Combat,
	Frozen,
	OnFire,
	Immune,
	Dead
};

UENUM(BlueprintType)
enum class EAIMovementSpeed : uint8
{
	Walk,
	Jog,
	Sprint,
	Investigate,
	Combat
};

UENUM(BlueprintType)
enum class EAISense : uint8
{
	None,
	Sight,
	Hearing,
	Damage
};
