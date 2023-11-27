// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConnectedMeshComp.generated.h"

UCLASS()
class THECRUSADER_API AConnectedMeshComp : public AActor
{
	GENERATED_BODY()

public:
	AConnectedMeshComp();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
