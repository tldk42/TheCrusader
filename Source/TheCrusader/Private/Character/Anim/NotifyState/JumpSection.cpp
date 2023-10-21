// Written by jaegang lim


#include "Character/Anim/NotifyState/JumpSection.h"

#include "Character/TCGASCharacter.h"

void UJumpSection::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ATCGASCharacter* Character = Cast<ATCGASCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->SetComboPeriod(true);
		Character->SetJumpSectionNotify(this);
	}
}


void UJumpSection::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ATCGASCharacter* Character = Cast<ATCGASCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		if (Character->bComboEnabled)
		{
			Character->JumpSectionForCombo();
		}

		Character->SetComboPeriod(false);
		Character->SetJumpSectionNotify(nullptr);
		Character->bComboEnabled = false;
	}
}
