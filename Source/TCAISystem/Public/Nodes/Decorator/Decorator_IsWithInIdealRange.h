// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Decorator_IsWithInIdealRange.generated.h"

/**
 * 
 */
UCLASS()
class TCAISYSTEM_API UDecorator_IsWithInIdealRange : public UBTDecorator
{
	GENERATED_BODY()

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector IdealRange;
};
