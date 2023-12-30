// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "HasPatrolRoute.generated.h"

/**
 * 
 */
UCLASS()
class TCAISYSTEM_API UHasPatrolRoute : public UBTDecorator
{
	GENERATED_BODY()
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
