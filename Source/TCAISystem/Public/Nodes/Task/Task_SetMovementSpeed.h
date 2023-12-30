// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Data/AIDataStruct.h"
#include "Task_SetMovementSpeed.generated.h"

/**
 * 
 */
UCLASS()
class TCAISYSTEM_API UTask_SetMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(Category = Speed, EditAnywhere)
	EAIMovementSpeed MovementSpeed;
};
