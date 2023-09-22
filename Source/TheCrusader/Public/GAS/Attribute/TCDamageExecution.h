// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "TCDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API UTCDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UTCDamageExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
