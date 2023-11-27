// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TheCrusader.h"
#include "AIController_Base.generated.h"


class ATCGASCharacter;
class UAISenseConfig_Damage;
class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;
struct FAIStimulus;
class UBehaviorTreeComponent;

UCLASS()
class THECRUSADER_API AAIController_Base : public AAIController
{
	GENERATED_BODY()

public:
	AAIController_Base();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UFUNCTION()
	void OnUpdated(TArray<AActor*> const& UpdatedActors);

	bool CanSenseActor(AActor* Actor, EAISense SenseType, FAIStimulus& OutStimulus) const;

public:
	static const FName KEY_STATE;
	static const FName KEY_TARGET_ACTOR;

protected:
	TObjectPtr<ATCGASCharacter> OwnerPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | Perception")
	UAISenseConfig_Sight* Config_Sight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | Perception")
	UAISenseConfig_Hearing* Config_Hearing;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | Perception")
	UAISenseConfig_Damage* Config_Damage;
};
