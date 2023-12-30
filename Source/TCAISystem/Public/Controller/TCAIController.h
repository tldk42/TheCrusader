// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Data/AIDataStruct.h"
#include "TCAIController.generated.h"

class UBehaviorTreeComponent;
struct FAIStimulus;
class UAISenseConfig_Damage;
class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;

UCLASS()
class TCAISYSTEM_API ATCAIController : public AAIController
{
	GENERATED_BODY()

public:
	static const FName KEY_STATE;
	static const FName KEY_TARGET_ACTOR;

public:
	ATCAIController();

	UFUNCTION(BlueprintCallable)
	EAIState GetCurrentState() const;

	UFUNCTION(BlueprintCallable)
	void SetState(const EAIState NewState);

	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void SetStateAsPatrolState();

	UFUNCTION(BlueprintCallable)
	void SetStateAsInvestigatingState(const FVector& InterestedLocation);

	UFUNCTION(BlueprintCallable)
	void SetStateAsCombatState(AActor* TargetActor);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UFUNCTION()
	void OnUpdated(TArray<AActor*> const& UpdatedActors);

	bool CanSenseActor(AActor* Actor, EAISense SenseType, FAIStimulus& OutStimulus) const;

	void HandleSensedSight(AActor* Actor);

	void HandleSensedSound(const FVector& SoundFrom);

	void HandleSensedDamage(AActor* Actor);

public:
	virtual void Tick(float DeltaTime) override;

protected:
	TObjectPtr<ACharacter> OwnerPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | Perception")
	UAISenseConfig_Sight* Config_Sight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | Perception")
	UAISenseConfig_Hearing* Config_Hearing;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | Perception")
	UAISenseConfig_Damage* Config_Damage;
};
