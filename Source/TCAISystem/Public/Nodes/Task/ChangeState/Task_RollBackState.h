﻿// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Task_RollBackState.generated.h"

/**
 * 
 */
UCLASS()
class TCAISYSTEM_API UTask_RollBackState : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
