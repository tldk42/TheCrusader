// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RadialMenuSystem.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THECRUSADER_API URadialMenuSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	URadialMenuSystem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY()
	TSubclassOf<UUserWidget> RadialWidgetClass;

	UPROPERTY()
	UUserWidget* RadialWidget;
};
