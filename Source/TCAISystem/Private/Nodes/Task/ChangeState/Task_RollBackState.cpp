// Written by jaegang lim


#include "Nodes/Task/ChangeState/Task_RollBackState.h"

#include "AIController.h"
#include "Controller/TCAIController.h"

EBTNodeResult::Type UTask_RollBackState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ATCAIController* const OwnerController = Cast<ATCAIController>(OwnerComp.GetAIOwner());

	EBTNodeResult::Type Result = EBTNodeResult::Failed;

	if (OwnerController && OwnerController->GetPawn())
	{
		OwnerController->SetStateAsPatrolState();
		Result = EBTNodeResult::Succeeded;
	}
	return Result;
}
