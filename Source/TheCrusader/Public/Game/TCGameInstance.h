// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TCGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class THECRUSADER_API UTCGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& MapName);
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);

public:
	UPROPERTY()
	FName PlayerStartTag = FName();

	UPROPERTY()
	FString LoadSlotName = FString();

	UPROPERTY()
	int32 LoadSlotIndex = 0;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LoadingScreenWidget;
};
