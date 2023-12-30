// Written by jaegang lim


#include "Nodes/Decorator/Decorator_IsHoldingWeapon.h"

#include "AIController.h"
#include "Interface/AttackableAI.h"

bool UDecorator_IsHoldingWeapon::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* const OwnerController = OwnerComp.GetAIOwner();

	if (OwnerController && OwnerController->GetPawn())
	{
		if (APawn* AIPawn = OwnerController->GetPawn())
		{
			return IAttackableAI::Execute_IsEquippedWeapon(AIPawn);
		}
	}
	return false;
}
