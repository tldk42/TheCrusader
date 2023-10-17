// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "TCPhysicalAnimComp.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THECRUSADER_API UTCPhysicalAnimComp : public UPhysicalAnimationComponent
{
	GENERATED_BODY()

public:
	
	UTCPhysicalAnimComp();

	void HitReaction(const FHitResult& HitResult);

	void TogglePhyxsAnimation();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


private:
	UPROPERTY()
	USkeletalMeshComponent* MeshComponent;

	float HitReactionTimeRemaining;
};
