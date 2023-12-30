// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQSContext_AttackTarget.generated.h"

/**
 * 
 */
UCLASS()
class TCAISYSTEM_API UEQSContext_AttackTarget : public UEnvQueryContext
{
	GENERATED_BODY()

protected:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
