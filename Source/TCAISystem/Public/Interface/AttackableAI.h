// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackableAI.generated.h"

UINTERFACE()
class UAttackableAI : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TCAISYSTEM_API IAttackableAI
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EquipWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UnEquipWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsEquippedWeapon();
};
