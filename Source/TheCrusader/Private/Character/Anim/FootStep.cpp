// Written by jaegang lim


#include "..\..\..\Public\Character\Anim\FootStepNotify.h"
#include "Component/FootSFX/FootStepComponent.h"

void UFootStepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (UFootStepComponent* FootStepComponent = Cast<UFootStepComponent>(MeshComp->GetOwner()->
		GetComponentByClass(
			UFootStepComponent::StaticClass())))
	{
		FootStepComponent->SetAnimation(Animation);
		FootStepComponent->FootStep();
	}
}
