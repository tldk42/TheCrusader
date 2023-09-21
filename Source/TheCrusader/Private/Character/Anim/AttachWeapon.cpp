// Written by jaegang lim


#include "Character/Anim/AttachWeapon.h"

#include "Character/Balian.h"

void UAttachWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const ABalian* Player = Cast<ABalian>(MeshComp->GetOwner()))
	{
		Player->AttachToPelvis();
	}
}
