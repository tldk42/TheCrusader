// Written by jaegang lim


#include "Character/Anim/EquipWeapon.h"

#include "Character/Balian.h"
#include "Item/Weapon/Item_Weapon.h"

void UEquipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const ABalian* Player = Cast<ABalian>(MeshComp->GetOwner()))
	{
		if (AItem_Weapon* Weapon = Player->GetCurrentWeapon())
		{
			Weapon->AttachToComponent(Player->GetMesh(),
			                          FAttachmentTransformRules::SnapToTargetIncludingScale,
			                          Weapon->GetItemData()->EquipmentData.
			                                  AttachmentSocket);
		}
	}
}
