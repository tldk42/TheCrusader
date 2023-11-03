// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheCrusaderGameMode.h"

#include "Character/Balian.h"
#include "UObject/ConstructorHelpers.h"

ATheCrusaderGameMode::ATheCrusaderGameMode()
{
}

void ATheCrusaderGameMode::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacterRef = CastChecked<ABalian>(GetWorld()->GetFirstPlayerController()->GetPawn());
}
