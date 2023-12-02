// Written by jaegang lim


#include "Game/LoadScreenSaveGame.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Interfaces/SaveInterface.h"

void ULoadScreenSaveGame::ActorArraySavor(TArray<AActor*>& SaveActors)
{
	for (AActor* SaveActor : SaveActors)
	{
		if (!IsValid(SaveActor) || !SaveActor->Implements<USaveInterface>()) continue;
		ActorSaver(SaveActor);
	}
}

void ULoadScreenSaveGame::ActorSaver(AActor* SaveActor)
{
	int MapIndex = 0;
	GetSavedMapWithMapName(MapAssetName, MapIndex);

	FSavedActor NewSavedActor;
	NewSavedActor.Name = SaveActor->GetFName();
	NewSavedActor.Transform = SaveActor->GetTransform();
	NewSavedActor.Class = SaveActor->GetClass();
	NewSavedActor.bActor = true;

	if (SavedMaps.IsEmpty())
	{
		FSavedMap Map;
		Map.MapAssetName = MapAssetName;
		Map.SavedActors.Empty();
		SavedMaps.Emplace();
	}

	SaveData(SaveActor, NewSavedActor.Data);

	const int32 ActorIndex = SavedMaps[MapIndex].SavedActors.Find(NewSavedActor);
	if (ActorIndex == INDEX_NONE)
	{
		SavedMaps[MapIndex].SavedActors.Emplace(NewSavedActor);
	}
	else
	{
		SavedMaps[MapIndex].SavedActors[ActorIndex] = NewSavedActor;
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
	MyArchive.ArIsSaveGame = true;

	Object->Serialize(MyArchive);
}

void ULoadScreenSaveGame::LoadData(UObject* Object, TArray<uint8>& Data)
{
	if (Object == nullptr)
		return;

	FMemoryReader MemoryReader(Data);

	FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
	Archive.ArIsSaveGame = true;
	Object->Serialize(Archive);
}

FSavedMap ULoadScreenSaveGame::GetSavedMapWithMapName(const FString& InMapName, int& Index)
{
	for (int i = 0; SavedMaps.IsValidIndex(i); ++i)
	{
		if (SavedMaps[i].MapAssetName == InMapName)
		{
			Index = i;
			return SavedMaps[i];
		}
	}
	Index = 0;
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
