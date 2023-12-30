// Written by jaegang lim


#include "Task_UnEquipWeapon.h"

#include "AIController.h"
#include "Interface/AttackableAI.h"

EBTNodeResult::Type UTask_UnEquipWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* const OwnerController = OwnerComp.GetAIOwner();

	EBTNodeResult::Type Result = EBTNodeResult::Failed;

	if (OwnerController && OwnerController->GetPawn())
	{
		if (APawn* AIPawn = OwnerController->GetPawn())
		{
			IAttackableAI::Execute_UnEquipWeapon(AIPawn);
			Result = EBTNodeResult::Succeeded;
		}
	}

	return Result;
}
