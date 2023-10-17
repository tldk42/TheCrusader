// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthHUD.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHealthHUD : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THECRUSADER_API IHealthHUD
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetHP(float InPercent);
};
