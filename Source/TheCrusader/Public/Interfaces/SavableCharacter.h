// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SavableCharacter.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USavableCharacter : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THECRUSADER_API ISavableCharacter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	int32 GetStatPoints() const;
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSkillPoints() const;

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowSaveWidget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideSaveWidget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SaveProgress(const FName& CheckpointTag);
};
