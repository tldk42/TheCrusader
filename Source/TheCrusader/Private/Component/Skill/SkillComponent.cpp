// Written by jaegang lim


#include "Component/Skill/SkillComponent.h"

#include "Engine/DataTable.h"
#include "Game/Skill_SaveGame.h"
#include "Game/TCGameInstance.h"
#include "Kismet/GameplayStatics.h"


USkillComponent::USkillComponent()
	: SkillDataTable(nullptr),
	  AvailablePoints(0)
{
	SkillsSlot = TEXT("SavedSkills");
}

void USkillComponent::SaveSkills()
{
	if (!IsValid(GetOwner()))
		return;

	if (const UTCGameInstance* GameInstance = Cast<UTCGameInstance>(UGameplayStatics::GetGameInstance(GetOwner())))
	{
		USkill_SaveGame* SaveGame;

		if (GameInstance->LoadSlotIndex == -1)
		{
			SkillsSlot.Empty();
			SkillsSlot = "Skills_Just For Test";
		}
		else
		{
			SkillsSlot = "SavedSkills";
			SkillsSlot.Append(FString::FromInt(GameInstance->LoadSlotIndex));
		}
		if (UGameplayStatics::DoesSaveGameExist(SkillsSlot, GameInstance->LoadSlotIndex))
		{
			SaveGame = Cast<USkill_SaveGame>(
				UGameplayStatics::LoadGameFromSlot(SkillsSlot, GameInstance->LoadSlotIndex));
		}
		else
		{
			SaveGame = Cast<USkill_SaveGame>(UGameplayStatics::CreateSaveGameObject(USkill_SaveGame::StaticClass()));
		}
		SaveGame->SavedSkills.PerksLists = PerksList;
		SaveGame->SavedSkills.SkillPoints = AvailablePoints;

		UGameplayStatics::SaveGameToSlot(SaveGame, SkillsSlot, GameInstance->LoadSlotIndex);
	}
}

bool USkillComponent::LoadSkills()
{
	if (!IsValid(GetOwner()))
		return false;

	if (const UTCGameInstance* GameInstance = Cast<UTCGameInstance>(UGameplayStatics::GetGameInstance(GetOwner())))
	{
		if (GameInstance->LoadSlotIndex == -1)
		{
			SkillsSlot.Empty();
			SkillsSlot = "Skills_Just For Test";
		}
		else
		{
			SkillsSlot = "SavedSkills";
			SkillsSlot.Append(FString::FromInt(GameInstance->LoadSlotIndex));
		}
		if (UGameplayStatics::DoesSaveGameExist(SkillsSlot, GameInstance->LoadSlotIndex))
		{
			const USkill_SaveGame* SavedSkills = Cast<USkill_SaveGame>(
				UGameplayStatics::LoadGameFromSlot(SkillsSlot, GameInstance->LoadSlotIndex));

			PerksList = SavedSkills->SavedSkills.PerksLists;
			AvailablePoints = SavedSkills->SavedSkills.SkillPoints;
			return true;
		}
	}
	return false;
}

void USkillComponent::RestoreSavedSkills(const FDataTableRowHandle& Row)
{
	SetPerkToTrue(Row);
}


FSkill USkillComponent::GetSkillInfo(const FDataTableRowHandle& SkillRowHandle)
{
	FSkill SkillInfo;
	if (!SkillRowHandle.IsNull())
	{
		SkillInfo = *SkillRowHandle.DataTable->FindRow<FSkill>(SkillRowHandle.RowName,
		                                                       SkillRowHandle.RowName.ToString());
	}
	return SkillInfo;
}


void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<FName> RowBases = SkillDataTable->GetRowNames();

	for (const FName RowBase : RowBases)
	{
		FDataTableRowHandle SkillRow;
		SkillRow.DataTable = SkillDataTable;
		SkillRow.RowName = RowBase;

		PerksList.Add({SkillRow, false});
	}
	if (LoadSkills())
	{
		for (const auto [RowHandle, bLearned] : PerksList)
		{
			if (bLearned)
			{
				ApplyChanges(RowHandle);
			}
		}
	}
}

void USkillComponent::ApplyChanges(const FDataTableRowHandle& RowHandle)
{
	const FSkill SkillInfo = GetSkillInfo(RowHandle);

	// TODO: Attribute에 적용시키거나 Ability 추가하기
	switch (SkillInfo.SkillType)
	{
	case ESkillType::Health:
		break;
	case ESkillType::Stamina:
		break;
	case ESkillType::Armor:
		break;
	case ESkillType::HealthRegen:
		break;
	case ESkillType::StaminaRegen:
		break;
	case ESkillType::LightAttackPower:
		break;
	case ESkillType::HardAttackPower:
		break;
	case ESkillType::AbilitySkill:
		break;
	case ESkillType::Magic:
		break;
	case ESkillType::Other:
		break;
	default: ;
	}
}

void USkillComponent::LearnSkill_Implementation(const FDataTableRowHandle& SkillRowHandle)
{
	const FSkill SkillInfo = GetSkillInfo(SkillRowHandle);
	if (AvailablePoints >= SkillInfo.RequiredPoint)
	{
		ApplyChanges(SkillRowHandle);
		SetPerkToTrue(SkillRowHandle);
		ControlPoints(false, SkillInfo.RequiredPoint);
		SaveSkills();
	}
	else
	{
		// FAILED EVENT START
	}
}


void USkillComponent::SetPerkToTrue(const FDataTableRowHandle& SkillRowHandle)
{
	for (int Index = 0; PerksList.IsValidIndex(Index); ++Index)
	{
		if (PerksList[Index].RowHandle.RowName == SkillRowHandle.RowName)
		{
			PerksList[Index].bLearned = true;
		}
	}
}

void USkillComponent::ControlPoints(const bool bPlus, const int Amount)
{
	if (bPlus)
	{
		AvailablePoints += Amount;
	}
	else
	{
		AvailablePoints = FMath::Clamp(AvailablePoints - Amount, 0, INT8_MAX);
	}
}
