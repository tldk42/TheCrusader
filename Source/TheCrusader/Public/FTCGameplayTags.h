// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
* FTCGameplayTags
*
*	Singleton containing native gameplay tags.
*/
class THECRUSADER_API FTCGameplayTags
{
public:
	static const FTCGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeTags();

	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_Spacebar;
	FGameplayTag InputTag_E;
	FGameplayTag InputTag_Dodge;
	FGameplayTag InputTag_Roll;
	FGameplayTag InputTag_MouseMiddle;
	FGameplayTag InputTag_MouseWheel;
	FGameplayTag InputTag_Crouch;
	FGameplayTag InputTag_RMB_Short;
	FGameplayTag InputTag_Num1;
	FGameplayTag InputTag_Num2;
	FGameplayTag InputTag_Num3;
	FGameplayTag InputTag_Num4;
	FGameplayTag InputTag_Num5;

protected:
	void AddAllTags(UGameplayTagsManager& Manager);

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:
	static FTCGameplayTags GameplayTags;
};
