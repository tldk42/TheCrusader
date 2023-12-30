// Written by jaegang lim


#include "Nodes/Task/Task_EquipWeapon.h"

#include "AIController.h"
#include "Interface/AttackableAI.h"


EBTNodeResult::Type UTask_EquipWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* const OwnerController = OwnerComp.GetAIOwner();

	EBTNodeResult::Type Result = EBTNodeResult::Failed;

	if (OwnerController && OwnerController->GetPawn())
	{
		if (APawn* AIPawn = OwnerController->GetPawn())
		{
			IAttackableAI::Execute_EquipWeapon(AIPawn);
			Result = EBTNodeResult::Succeeded;
		}
	}

	return Result;
}
