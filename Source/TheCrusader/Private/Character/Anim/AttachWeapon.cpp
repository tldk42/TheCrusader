// Written by jaegang lim


#include "Character/Anim/AttachWeapon.h"

#include "Character/Balian.h"
#include "Item/Weapon/Item_Weapon.h"

void UAttachWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const ABalian* Player = Cast<ABalian>(MeshComp->GetOwner()))
	{
		Player->GetCurrentWeapon()->AttachToComponent(Player->GetMesh(),
		                                              FAttachmentTransformRules::SnapToTargetIncludingScale,
		                                              Player->GetCurrentWeapon()->GetItemData()->WeaponData.
		                                                      AttachmentSocket);
	}
}
