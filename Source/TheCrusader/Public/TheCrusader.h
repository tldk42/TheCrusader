// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Character gait state. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType)
enum class ETCGait : uint8
{
	Walking,
	Running,
	Sprinting
};

/**
 * Character rotation mode. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType)
enum class ETCRotationMode : uint8
{
	VelocityDirection,
	LookingDirection,
	Aiming
};

/**
 * Character view mode. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType)
enum class ETCViewMode : uint8
{
	ThirdPerson,
	FirstPerson
};

/**
 * Character stance. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType)
enum class ETCStance : uint8
{
	Standing,
	Crouching
};

/**
 * Character movement action state. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType)
enum class ETCMovementAction : uint8
{
	None,
	LowMantle,
	HighMantle,
	Rolling,
	GettingUp
};

/**
 * Character movement state. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType)
enum class ETCMovementState : uint8
{
	None,
	Grounded,
	InAir,
	Mantling,
	Ragdoll
};

UENUM(Blueprintable, BlueprintType)
enum class ECharacterState : uint8
{
	Idle,
	Patrolling,
	Investigating,
	Chasing,
	Combat,
	Blocking,
	Shocked
};

UENUM(BlueprintType, Blueprintable)
enum class EAISense : uint8
{
	None,
	Sight,
	Hearing,
	Damage
};
