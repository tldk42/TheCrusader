// Copyright Epic Games, Inc. All Rights Reserved.


#include "Game/TheCrusaderGameMode.h"

#include "EngineUtils.h"
#include "Character/Balian.h"
#include "Game/LoadScreenSaveGame.h"
#include "Game/TCGameInstance.h"
#include "GameFramework/PlayerStart.h"
#include "Interfaces/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "UObject/ConstructorHelpers.h"


void ATheCrusaderGameMode::DeleteSlot(const FString& SlotName, const int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

AActor* ATheCrusaderGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	const UTCGameInstance* TCGameInstance = Cast<UTCGameInstance>(GetGameInstance());

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);

	if (Actors.Num() > 0)
	{
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == TCGameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
}

void ATheCrusaderGameMode::SaveSlotData(UMVVM_LoadSlot* LoadSlot, const int8 SlotIndex) const
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->LoadSlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->LoadSlotName, SlotIndex);
	}
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SaveSlotStatus = Taken;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->MapAssetName = LoadSlot->MapAssetName;
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex);
}

void ATheCrusaderGameMode::TravelToMap(const UMVVM_LoadSlot* Slot)
{
	const FString SlotName = Slot->LoadSlotName;
	// const int32 SlotIndex = Slot->SlotIndex;

	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

void ATheCrusaderGameMode::SaveInGameProgressData(ULoadScreenSaveGame* SaveObject) const
{
	UTCGameInstance* TCGameInstance = Cast<UTCGameInstance>(GetGameInstance());

	const FString InGameLoadSlotName = TCGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = TCGameInstance->LoadSlotIndex;
	TCGameInstance->PlayerStartTag = SaveObject->PlayerStartTag;
	const ULoadScreenSaveGame* SaveGame = GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
	SaveObject->MapAssetName = SaveGame->MapAssetName;
	SaveObject->MapName = SaveGame->MapName;
	SaveObject->SavedMaps = SaveGame->SavedMaps;

	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

void ATheCrusaderGameMode::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UTCGameInstance* TCGameInstance = Cast<UTCGameInstance>(GetGameInstance());
	check(TCGameInstance);

	if (ULoadScreenSaveGame* SaveGame = GetSaveSlotData(TCGameInstance->LoadSlotName, TCGameInstance->LoadSlotIndex))
	{
		if (DestinationMapAssetName != FString(""))
		{
			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->MapName = GetMapNameFromMapAssetName(DestinationMapAssetName);
		}

		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);
		}

		FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.SavedActors.Empty();

		/** FActorIterator를 이용하면 원하는 타입의 액터를 선별*/
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			/** 유효한 액터와 SaveInterface를 상속받은 액터들에 대해서만 검사한다.*/
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			/** 유효한 액터들의 이름과 위치를 저장한다.*/
			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();

			/** FMemoryWriter를 이용해서 파일에 저장한다.*/
			FMemoryWriter MemoryWriter(SavedActor.Bytes);

			/** FObjectAndNameAsStringProxyArchive
			 * 객체와 이름을 문자열로 직렬화하는 역할 */
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true;

			/** 액터의 상태를 Archive에 직렬화 (MemoryWriter -> SavecActor.Bytes에 저장됨)*/
			Actor->Serialize(Archive);

			SavedMap.SavedActors.AddUnique(SavedActor);
		}

		// for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		// {
		// 	if (MapToReplace.MapAssetName == WorldName)
		// 	{
		// 		MapToReplace = SavedMap;
		// 	}
		// }
		UGameplayStatics::SaveGameToSlot(SaveGame, TCGameInstance->LoadSlotName, TCGameInstance->LoadSlotIndex);
	}
}

void ATheCrusaderGameMode::SaveActor(AActor* Actor) const
{
	if (UTCGameInstance* TCGameInstance = Cast<UTCGameInstance>(GetGameInstance()))
	{
		const FString InGameLoadSlotName = TCGameInstance->LoadSlotName;
		const int32 InGameLoadSlotIndex = TCGameInstance->LoadSlotIndex;

		if (ULoadScreenSaveGame* SaveGame = GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex))
		{
			FString WorldName = GetWorld()->GetMapName();
			WorldName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
			FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);

			if (SavedMap.MapAssetName.IsEmpty())
			{
				SavedMap.MapAssetName = WorldName;
				SavedMap.SavedActors.Empty();

				FSavedActor SavedActor;
				SavedActor.ActorName = Actor->GetFName();
				SavedActor.Transform = Actor->GetTransform();

				FMemoryWriter MemoryWriter(SavedActor.Bytes);

				FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
				Archive.ArIsSaveGame = true;

				Actor->Serialize(Archive);
				SavedMap.SavedActors.AddUnique(SavedActor);

				SaveGame->SavedMaps.Add(SavedMap);
				

				UGameplayStatics::SaveGameToSlot(SaveGame, TCGameInstance->LoadSlotName, TCGameInstance->LoadSlotIndex);
				return;
			}

			for (FSavedActor& SavedActor : SavedMap.SavedActors)
			{
				if (SavedActor.ActorName == GetFName())
				{
					SavedActor.Transform = Actor->GetTransform();

					FMemoryWriter MemoryWriter(SavedActor.Bytes);

					FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
					Archive.ArIsSaveGame = true;

					Actor->Serialize(Archive);
					UGameplayStatics::SaveGameToSlot(SaveGame, TCGameInstance->LoadSlotName,
					                                 TCGameInstance->LoadSlotIndex);
					return;
				}
			}
		}
	}
}

void ATheCrusaderGameMode::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	const UTCGameInstance* TCGameInstance = Cast<UTCGameInstance>(GetGameInstance());
	check(TCGameInstance);

	if (UGameplayStatics::DoesSaveGameExist(TCGameInstance->LoadSlotName, TCGameInstance->LoadSlotIndex))
	{
		ULoadScreenSaveGame* SaveGame = Cast<ULoadScreenSaveGame>(
			UGameplayStatics::LoadGameFromSlot(TCGameInstance->LoadSlotName, TCGameInstance->LoadSlotIndex));
		if (!SaveGame)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load slot"));
			return;
		}

		for (FActorIterator Iterator(World); Iterator; ++Iterator)
		{
			if (AActor* Actor = *Iterator; Actor->Implements<USaveInterface>())
			{
				FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
				for (const FSavedActor& SavedActor : SavedMap.SavedActors)
				{
					if (SavedActor.ActorName == Actor->GetFName())
					{
						if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
						{
							Actor->SetActorTransform(SavedActor.Transform);
						}

						FMemoryReader MemoryReader(SavedActor.Bytes);

						FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
						Archive.ArIsSaveGame = true;
						Actor->Serialize(Archive);

						ISaveInterface::Execute_LoadActor(Actor);
					}
				}
			}
		}
	}
}

void ATheCrusaderGameMode::PlayerDied(const ACharacter* DeadCharacter) const
{
	const ULoadScreenSaveGame* SaveGame = RetrieveInGameSaveData();
	if (!IsValid(SaveGame))
	{
		return;
	}

	UGameplayStatics::OpenLevel(DeadCharacter, FName(SaveGame->MapAssetName));
}

ULoadScreenSaveGame* ATheCrusaderGameMode::GetSaveSlotData(const FString& SlotName, const int32 SlotIndex) const
{
	USaveGame* SaveGame = UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex)
		                      ? UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex)
		                      : UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);

	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGame);

	return LoadScreenSaveGame;
}

FString ATheCrusaderGameMode::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (auto& Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}

ULoadScreenSaveGame* ATheCrusaderGameMode::RetrieveInGameSaveData() const
{
	const UTCGameInstance* TCGameInstance = Cast<UTCGameInstance>(GetGameInstance());

	const FString InGameLoadSlotName = TCGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = TCGameInstance->LoadSlotIndex;

	return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
}

void ATheCrusaderGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetFirstPlayerController()->GetPawn()->IsA(ABalian::StaticClass()))
	{
		PlayerCharacterRef = Cast<ABalian>(GetWorld()->GetFirstPlayerController()->GetPawn());
	}

	Maps.Add(DefaultMapName, DefaultMap);
}
