// Written by jaegang lim


#include "Player/Camera/TCPlayerCameraBehavior.h"

void UTCPlayerCameraBehavior::SetRotationMode(ETCRotationMode RotationMode)
{
	bVelocityDirection = RotationMode == ETCRotationMode::VelocityDirection;
	bLookingDirection = RotationMode == ETCRotationMode::LookingDirection;
	bAiming = RotationMode == ETCRotationMode::Aiming;
}
