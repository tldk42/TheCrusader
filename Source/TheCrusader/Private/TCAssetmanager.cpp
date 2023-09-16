// Written by jaegang lim


#include "TCAssetmanager.h"

#include "FTCGameplayTags.h"

UTCAssetmanager::UTCAssetmanager()
{
}

UTCAssetmanager& UTCAssetmanager::Get()
{
	check(GEngine);

	UTCAssetmanager* Assetmanager = Cast<UTCAssetmanager>(GEngine->AssetManager);

	return *Assetmanager;
}

void UTCAssetmanager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FTCGameplayTags::InitializeNativeTags();
}
