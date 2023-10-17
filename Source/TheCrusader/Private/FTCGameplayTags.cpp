// Written by jaegang lim


#include "FTCGameplayTags.h"
#include "GameplayTagsManager.h"
#include "Engine/EngineTypes.h"

FTCGameplayTags FTCGameplayTags::GameplayTags;

void FTCGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();

	GameplayTags.AddAllTags(GameplayTagsManager);

	GameplayTagsManager.DoneAddingNativeTags();
}

void FTCGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InputTag_LMB, "InputTag.LMB", "Left Mouse Button");
	AddTag(InputTag_RMB, "InputTag.RMB", "Right Mouse Button");
	AddTag(InputTag_Spacebar, "InputTag.Spacebar", "Spacebar Button");
	AddTag(InputTag_E, "InputTag.E", "E Button");
	AddTag(InputTag_Dodge, "InputTag.LAlt", "LAlt Button");
	AddTag(InputTag_Roll, "InputTag.Roll", "LAlt Button(Roll)");
	AddTag(InputTag_MouseMiddle, "InputTag.MouseMiddle", "Mouse Middle");
	AddTag(InputTag_MouseWheel, "InputTag.MouseWheel", "Mouse Wheel");
	AddTag(InputTag_Numpad1, "InputTag.Num1", "NumPad1");
}

void FTCGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName),
	                                                          FString(TEXT("(Native) ")) + FString(TagComment));
}
