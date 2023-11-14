// Written by jaegang lim


#include "Game/TCGameInstance.h"

#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"

void UTCGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::EndLoadingScreen);
}

void UTCGameInstance::BeginLoadingScreen(const FString& MapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;

		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenWidget);
		LoadingScreen.WidgetLoadingScreen = Widget->TakeWidget();
 
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}

void UTCGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
}
