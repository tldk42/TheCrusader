// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "ParkourAnimation.generated.h"

struct FGameplayTag;
// This class does not need to be modified.
UINTERFACE()
class UParkourAnimation : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PARKOUR_API IParkourAnimation
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool SetParkourState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool SetParkourAction(const FGameplayTag& NewAction);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool SetClimbStyle(const FGameplayTag& NewClimbStyle);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool SetClimbDirection(const FGameplayTag& NewClimbDirection);
};
