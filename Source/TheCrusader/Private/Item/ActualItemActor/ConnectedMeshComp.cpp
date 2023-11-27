// Written by jaegang lim


#include "Item/ActualItemActor/ConnectedMeshComp.h"


// Sets default values
AConnectedMeshComp::AConnectedMeshComp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AConnectedMeshComp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AConnectedMeshComp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

