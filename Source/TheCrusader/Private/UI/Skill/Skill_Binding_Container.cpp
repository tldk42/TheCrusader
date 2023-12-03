// Written by jaegang lim


#include "UI/Skill/Skill_Binding_Container.h"

#include "Components/HorizontalBox.h"
#include "Game/TCGameInstance.h"
#include "Game/UI_SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Skill/Skill_Slot_Equipped.h"

void USkill_Binding_Container::SaveData() const
{
	if (!IsValid(GetOwningPlayer()))
		return;
	if (const UTCGameInstance* GameInstance = Cast<UTCGameInstance>(GetGameInstance()))
	{
		FString UISlot = TEXT("SavedUI");

		if (GameInstance->LoadSlotIndex == -1)
		{
			UISlot.Empty();
			UISlot = "UI_Just For Test";
		}
		else
		{
			UISlot = "SavedUI";
			UISlot.Append(FString::FromInt(GameInstance->LoadSlotIndex));
		}

		UUI_SaveGame* SaveGame;

		if (UGameplayStatics::DoesSaveGameExist(UISlot, GameInstance->LoadSlotIndex))
		{
			SaveGame = Cast<UUI_SaveGame>(
				UGameplayStatics::LoadGameFromSlot(UISlot, GameInstance->LoadSlotIndex));
		}
		else
		{
			SaveGame = Cast<UUI_SaveGame>(UGameplayStatics::CreateSaveGameObject(UUI_SaveGame::StaticClass()));
		}

		SaveGame->SavedUIData.Empty();

		for (UWidget* SkillSlot : PassiveSkills->GetAllChildren())
		{
			const USkill_Slot_Equipped* Slot_Equipped = Cast<USkill_Slot_Equipped>(SkillSlot);
			FSavedSkillUIData NewUIData = Slot_Equipped->Data;
			SaveGame->SavedUIData.Emplace(Slot_Equipped->SlotIndex, NewUIData);
		}

		for (UWidget* SkillSlot : ActiveSkills->GetAllChildren())
		{
			const USkill_Slot_Equipped* Slot_Equipped = Cast<USkill_Slot_Equipped>(SkillSlot);
			FSavedSkillUIData NewUIData = Slot_Equipped->Data;
			SaveGame->SavedUIData.Emplace(Slot_Equipped->SlotIndex, NewUIData);
		}

		UGameplayStatics::SaveGameToSlot(SaveGame, UISlot, GameInstance->LoadSlotIndex);
	}
}

void USkill_Binding_Container::LoadData() const
{
	if (!IsValid(GetOwningPlayer()))
		return;

	if (const UTCGameInstance* GameInstance = Cast<UTCGameInstance>(GetGameInstance()))
	{
		FString UISlot = TEXT("SavedUI");

		if (GameInstance->LoadSlotIndex == -1)
		{
			UISlot.Empty();
			UISlot = "UI_Just For Test";
		}
		else
		{
			UISlot = "SavedUI";
			UISlot.Append(FString::FromInt(GameInstance->LoadSlotIndex));
		}
		if (UGameplayStatics::DoesSaveGameExist(UISlot, GameInstance->LoadSlotIndex))
		{
			const UUI_SaveGame* SavedUI = Cast<UUI_SaveGame>(
				UGameplayStatics::LoadGameFromSlot(UISlot, GameInstance->LoadSlotIndex));


			for (UWidget*& SkillSlot : PassiveSkills->GetAllChildren())
			{
				USkill_Slot_Equipped* Slot_Equipped = Cast<USkill_Slot_Equipped>(SkillSlot);
				Slot_Equipped->Data = SavedUI->SavedUIData[Slot_Equipped->SlotIndex];
				Slot_Equipped->UpdateSlot();
				// SkillSlot = Slot_Equipped;
			}

			for (UWidget*& SkillSlot : ActiveSkills->GetAllChildren())
			{
				USkill_Slot_Equipped* Slot_Equipped = Cast<USkill_Slot_Equipped>(SkillSlot);
				Slot_Equipped->Data = SavedUI->SavedUIData[Slot_Equipped->SlotIndex];
				Slot_Equipped->UpdateSlot();
				// SkillSlot = Slot_Equipped;
			}
		}
	}
}
