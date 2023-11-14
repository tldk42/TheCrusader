// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TheCrusaderGameMode.generated.h"

class UMVVM_LoadSlot;
class USaveGame;
class ULoadScreenSaveGame;
class ABalian;

UCLASS()
class ATheCrusaderGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);

public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int8 SlotIndex) const;
	void SaveInGameProgressData(ULoadScreenSaveGame* SaveObject) const;
	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	
	void LoadWorldState(UWorld* World) const;

	void TravelToMap(const UMVVM_LoadSlot* Slot);

	void PlayerDied(const ACharacter* DeadCharacter) const;
	
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;
	ULoadScreenSaveGame* RetrieveInGameSaveData() const;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly)
	ABalian* PlayerCharacterRef;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

};
