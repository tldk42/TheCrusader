// Written by jaegang lim


#include "UI/Skill/Skill_Slot_Equipped.h"

#include "Character/Balian.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "GAS/Ability/TCGameplayAbility.h"
#include "UI/Skill/Skill_Information.h"


void USkill_Slot_Equipped::NativePreConstruct()
{
	Super::NativePreConstruct();

	Icon->SetColorAndOpacity(FColor::Transparent);
	Border->SetBrushColor(bIsPassive ? FColor::Orange : FColor::Magenta);
}

void USkill_Slot_Equipped::NativeConstruct()
{
	Super::NativeConstruct();
}


void USkill_Slot_Equipped::UpdateSlot_Implementation()
{
	if (Data.AbilityClass)
	{
		Icon->SetColorAndOpacity(Data.bAllocated ? FColor::White : FColor::Transparent);
		Icon->SetBrushFromTexture(Data.Icon);
		if (ABalian* Player = Cast<ABalian>(GetOwningPlayerPawn()))
		{
			Player->AddSkill(SlotIndex, Data.AbilityClass);
		}
		OnSlotUpdated.Broadcast();
	}
}

void USkill_Slot_Equipped::DeleteSlot_Implementation()
{
	if (Data.AbilityClass)
	{
		Icon->SetColorAndOpacity(FColor::Transparent);
		Icon->SetBrushFromTexture(nullptr);

		if (ABalian* Player = Cast<ABalian>(GetOwningPlayerPawn()))
		{
			Player->AddSkill(SlotIndex, nullptr);
		}
		Data.bAllocated = false;
		Data.Icon = nullptr;
		Data.Name = FString();
		Data.AbilityClass = nullptr;

		OnSlotUpdated.Broadcast();
	}
}
