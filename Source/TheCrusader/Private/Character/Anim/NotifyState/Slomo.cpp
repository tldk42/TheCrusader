// Written by jaegang lim


#include "Character/Anim/NotifyState/Slomo.h"

#include "Kismet/GameplayStatics.h"

void USlomo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (bForceSlomo)
	{
		UGameplayStatics::SetGlobalTimeDilation(MeshComp, BeginSpeed);
	}
}

void USlomo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (bForceSlomo)
	{
		UGameplayStatics::SetGlobalTimeDilation(MeshComp, EndSpeed);
	}
}
