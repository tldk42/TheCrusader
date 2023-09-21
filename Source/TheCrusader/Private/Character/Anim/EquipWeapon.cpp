// Written by jaegang lim


#include "Character/Anim/EquipWeapon.h"

#include "Character/Balian.h"

void UEquipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const ABalian* Player = Cast<ABalian>(MeshComp->GetOwner()))
	{
		Player->EquipToHand();
	}
}
