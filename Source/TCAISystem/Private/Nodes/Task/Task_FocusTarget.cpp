// Written by jaegang lim


#include "Nodes/Task/Task_FocusTarget.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"

EBTNodeResult::Type UTask_FocusTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetAIOwner()->SetFocus(UBTFunctionLibrary::GetBlackboardValueAsActor(this, TargetKey));
	return EBTNodeResult::Succeeded;
}
