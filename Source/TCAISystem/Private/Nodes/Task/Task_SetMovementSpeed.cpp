// Written by jaegang lim


#include "Nodes/Task/Task_SetMovementSpeed.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "AIController.h"
#include "Interface/MovableAI.h"

EBTNodeResult::Type UTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* const OwnerController = OwnerComp.GetAIOwner();

	EBTNodeResult::Type Result = EBTNodeResult::Failed;

	if (OwnerController && OwnerController->GetPawn())
	{
		if (APawn* AIPawn = OwnerController->GetPawn())
		{
			IMovableAI::Execute_SetMovementSpeed(AIPawn, MovementSpeed);
			Result = EBTNodeResult::Succeeded;
		}
	}
	return Result;
}
