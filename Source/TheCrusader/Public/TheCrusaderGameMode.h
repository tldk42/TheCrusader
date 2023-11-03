// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TheCrusaderGameMode.generated.h"

class ABalian;

UCLASS(minimalapi)
class ATheCrusaderGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATheCrusaderGameMode();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly)
	ABalian* PlayerCharacterRef;
};
