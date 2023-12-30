// Written by jaegang lim


#include "Nodes/Decorator/HasPatrolRoute.h"

#include "AIController.h"
#include "Interface/MovableAI.h"
#include "Spline/Spline_Path.h"

bool UHasPatrolRoute::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* const OwnerController = OwnerComp.GetAIOwner();

	if (OwnerController && OwnerController->GetPawn())
	{
		if (APawn* AIPawn = OwnerController->GetPawn())
		{
			return IsValid(IMovableAI::Execute_GetSplineRoute(AIPawn));
		}
	}
	return false;
}
