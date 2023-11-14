// Written by jaegang lim


#include "Game/LoadScreenSaveGame.h"

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
