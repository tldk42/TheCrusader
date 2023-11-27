// Written by jaegang lim


#include "Game/LoadScreenSaveGame.h"

#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void ULoadScreenSaveGame::ActorArraySavor(TArray<AActor*>& SaveActors)
{
	for (AActor* SaveActor : SaveActors)
	{
		ActorSaver(SaveActor);
	}
}

void ULoadScreenSaveGame::ActorSaver(AActor* SaveActor)
{
	FSavedMap SavedMap = GetSavedMapWithMapName(MapAssetName);

	const int32 Index = SavedMap.SavedActors.Emplace();
	FSavedActor& NewSaveActor = SavedMap.SavedActors[Index];

	NewSaveActor.ActorName = SaveActor->GetFName();
	NewSaveActor.Transform = SaveActor->GetTransform();
	NewSaveActor.
}

void ULoadScreenSaveGame::ActorPreLoader(AActor* WorldActor, FSavedActor& ActorRecord)
{
}

void ULoadScreenSaveGame::UObjectArraySaver(TArray<UObject*>& SaveObjects)
{
	for (UObject* SaveObject : SaveObjects)
	{
		UObjectSaver(SaveObject);
	}
}

void ULoadScreenSaveGame::UObjectSaver(UObject* SaveObject)
{
	if (SaveObject == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Save Object!"))
		return;
	}

	if (SaveObject->HasAnyFlags(EObjectFlags::RF_Transient))
	{
		UE_LOG(LogTemp, Warning, TEXT("Saving RF_Transient object"))
		return;
	}
}

void ULoadScreenSaveGame::SaveData(UObject* Object, TArray<uint8>& Data)
{
	if (Object == nullptr)
		return;

	FMemoryWriter MemoryWriter = FMemoryWriter(Data, true);

	FObjectAndNameAsStringProxyArchive MyArchive = FObjectAndNameAsStringProxyArchive(MemoryWriter, true);

	Object->Serialize(MyArchive);
}

void ULoadScreenSaveGame::LoadData(UObject* Object, TArray<uint8>& Data)
{
	if (Object == nullptr)
		return;

	FMemoryReader MemoryReader(Data, true);

	FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
	Object->Serialize(Archive);
}

FSavedMap ULoadScreenSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	for (const FSavedMap& Map : SavedMaps)
	{
		if (Map.MapAssetName == InMapName)
		{
			return Map;
		}
	}

	return FSavedMap();
}

bool ULoadScreenSaveGame::HasMap(const FString& InMapName)
{
	for (const auto& [SavedMapAssetName, SavedActors] : SavedMaps)
	{
		if (SavedMapAssetName == InMapName)
		{
			return true;
		}
	}
	return false;
}
