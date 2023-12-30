// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Data/AIDataStruct.h"
#include "UObject/Interface.h"
#include "MovableAI.generated.h"

class UBehaviorTree;

UINTERFACE()
class UMovableAI : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TCAISYSTEM_API IMovableAI
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UBehaviorTree* GetBehaviorTree() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class ASpline_Path* GetSplineRoute();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float SetMovementSpeed(const EAIMovementSpeed MovementSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetIdealRange(float& AttackRadius, float& DefenseRadius);
};
