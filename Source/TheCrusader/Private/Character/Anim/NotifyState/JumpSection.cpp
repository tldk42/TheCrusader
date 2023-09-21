﻿// Written by jaegang lim


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

void UJumpSection::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{

	// if (IsValid(Animation))
	// {
	// 	ATCGASCharacter* Character = Cast<ATCGASCharacter>(MeshComp->GetOwner());
	// 	if (Character)
	// 	{
	// 		Character->JumpSectionForCombo();
	// 	}
	// }
	
}

void UJumpSection::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

	ATCGASCharacter* Character = Cast<ATCGASCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->SetComboPeriod(false);
		Character->SetJumpSectionNotify(nullptr);
	}
}
