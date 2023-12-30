// Written by jaegang lim


#include "Nodes/Task/Task_ClearFocus.h"

#include "AIController.h"

EBTNodeResult::Type UTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetAIOwner()->K2_ClearFocus();

	return EBTNodeResult::Succeeded;
}
