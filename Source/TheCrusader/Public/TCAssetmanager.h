// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "TCAssetmanager.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API UTCAssetmanager : public UAssetManager
{
	GENERATED_BODY()

public:

	UTCAssetmanager();

	static UTCAssetmanager& Get();

protected:

	virtual void StartInitialLoading() override;
};
