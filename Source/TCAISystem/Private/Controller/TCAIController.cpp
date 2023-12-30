// Written by jaegang lim


#include "Controller/TCAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Interface/MovableAI.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

const FName ATCAIController::KEY_STATE(TEXT("State"));
const FName ATCAIController::KEY_TARGET_ACTOR(TEXT("AttackTarget"));

ATCAIController::ATCAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>("Blackboard Component");
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	Config_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("AI Sense Sight");
	Config_Sight->SightRadius = 800.f;
	Config_Sight->LoseSightRadius = 1200.f;
	Config_Sight->PeripheralVisionAngleDegrees = 60.f;
	Config_Sight->SetMaxAge(.1f);
	// Config_Sight->AutoSuccessRangeFromLastSeenLocation = 900.f;
	Config_Sight->DetectionByAffiliation.bDetectEnemies = true;
	Config_Sight->DetectionByAffiliation.bDetectFriendlies = true;
	Config_Sight->DetectionByAffiliation.bDetectNeutrals = true;

	Config_Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>("AI Sense Hearing");
	Config_Hearing->HearingRange = 1200.f;
	Config_Hearing->SetMaxAge(3.f);
	Config_Hearing->DetectionByAffiliation.bDetectEnemies = true;
	Config_Hearing->DetectionByAffiliation.bDetectFriendlies = true;
	Config_Hearing->DetectionByAffiliation.bDetectNeutrals = true;

	Config_Damage = CreateDefaultSubobject<UAISenseConfig_Damage>("AI Sense Damage");

	// 주도적인 감각을 Sight Sense로 설정
	PerceptionComponent->ConfigureSense(*Config_Sight);
	PerceptionComponent->ConfigureSense(*Config_Hearing);
	PerceptionComponent->ConfigureSense(*Config_Damage);
	PerceptionComponent->SetDominantSense(Config_Sight->GetSenseImplementation());
	PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ThisClass::OnUpdated);
}

EAIState ATCAIController::GetCurrentState() const
{
	return static_cast<EAIState>(BlackboardComponent->GetValueAsEnum("State"));
}

void ATCAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(Blackboard.Get()) && IsValid(BehaviorTree.Get()))
	{
		Blackboard->InitializeBlackboard(*BehaviorTree.Get()->BlackboardAsset.Get());
	}

	OwnerPawn = CastChecked<ACharacter>(GetPawn());
	BehaviorTree = IMovableAI::Execute_GetBehaviorTree(OwnerPawn);
}

void ATCAIController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(BehaviorTree.Get()))
	{
		ACharacter* PlayerCharacter = CastChecked<ACharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		RunBehaviorTree(BehaviorTree.Get());
		// BehaviorTreeComponent->StartTree(*BehaviorTree.Get());
		BlackboardComponent->SetValueAsObject("SelfActor", OwnerPawn);
		BlackboardComponent->SetValueAsObject(KEY_TARGET_ACTOR, PlayerCharacter);
		BlackboardComponent->SetValueAsEnum("State", static_cast<uint8>(EAIState::Patrol));

		float AttackRad, DefendRad;
		IMovableAI::Execute_GetIdealRange(OwnerPawn, AttackRad, DefendRad);

		BlackboardComponent->SetValueAsFloat("AttackRadius", AttackRad);
		BlackboardComponent->SetValueAsFloat("DefendRadius", DefendRad);
		// BlackboardComponent->SetValueAsObject("AbilitySystem", OwnerPawn->GetAbilitySystemComponent());
		// BlackboardComponent->SetValueAsObject("TargetAbilitySystem", PlayerCharacter->GetAbilitySystemComponent());
	}
}

void ATCAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ATCAIController::SetState(const EAIState NewState)
{
	GetBlackboardComponent()->SetValueAsEnum("State", static_cast<uint8>(NewState));
}

void ATCAIController::SetTarget(AActor* TargetActor)
{
	GetBlackboardComponent()->SetValueAsObject(KEY_TARGET_ACTOR, TargetActor);
}

void ATCAIController::SetStateAsPatrolState()
{
	GetBlackboardComponent()->SetValueAsEnum("State", static_cast<uint8>(EAIState::Patrol));
}

void ATCAIController::SetStateAsInvestigatingState(const FVector& InterestedLocation)
{
	GetBlackboardComponent()->SetValueAsEnum("State", static_cast<uint8>(EAIState::Investigating));
	GetBlackboardComponent()->SetValueAsVector("InvestigatingLocation", InterestedLocation);
}

void ATCAIController::SetStateAsCombatState(AActor* TargetActor)
{
	GetBlackboardComponent()->SetValueAsEnum("State", static_cast<uint8>(EAIState::Combat));
	SetTarget(TargetActor);
}


void ATCAIController::OnUpdated(TArray<AActor*> const& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		FAIStimulus AIStimulus;
		if (CanSenseActor(Actor, EAISense::Sight, AIStimulus))
		{
			HandleSensedSight(Actor);
		}
		if (CanSenseActor(Actor, EAISense::Hearing, AIStimulus))
		{
			HandleSensedSound(AIStimulus.StimulusLocation);
		}
		if (CanSenseActor(Actor, EAISense::Damage, AIStimulus))
		{
			HandleSensedDamage(Actor);
		}
	}
}

bool ATCAIController::CanSenseActor(AActor* Actor, const EAISense SenseType, FAIStimulus& OutStimulus) const
{
	FActorPerceptionBlueprintInfo Info;
	PerceptionComponent->GetActorsPerception(Actor, Info);

	for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
	{
		const TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus);
		TSubclassOf<UAISense> CompareToClass;

		switch (SenseType)
		{
		case EAISense::None:
			break;
		case EAISense::Sight:
			CompareToClass = UAISense_Sight::StaticClass();
			break;
		case EAISense::Hearing:
			CompareToClass = UAISense_Hearing::StaticClass();
			break;
		case EAISense::Damage:
			CompareToClass = UAISense_Damage::StaticClass();
			break;
		default: ;
		}

		if (CompareToClass == SenseClass)
		{
			OutStimulus = Stimulus;
			return Stimulus.WasSuccessfullySensed();
		}
	}
	return false;
}

void ATCAIController::HandleSensedSight(AActor* Actor)
{
	switch (GetCurrentState())
	{
	case EAIState::Idle:
	case EAIState::Patrol:
	case EAIState::Investigating:
		SetStateAsCombatState(Actor);
		break;
	case EAIState::Combat:
		break;
	case EAIState::Frozen:
	case EAIState::OnFire:
		break;
	case EAIState::Immune:
	case EAIState::Dead:
		break;
	}
}

void ATCAIController::HandleSensedSound(const FVector& SoundFrom)
{
	switch (GetCurrentState())
	{
	case EAIState::Idle:
	case EAIState::Patrol:
	case EAIState::Investigating:
		SetStateAsInvestigatingState(SoundFrom);
		break;
	case EAIState::Combat:
		break;
	case EAIState::Frozen:
		break;
	case EAIState::OnFire:
		break;
	case EAIState::Immune:
	case EAIState::Dead:
		break;
	}
}

void ATCAIController::HandleSensedDamage(AActor* Actor)
{
	switch (GetCurrentState())
	{
	case EAIState::Idle:
	case EAIState::Patrol:
	case EAIState::Investigating:
		SetStateAsCombatState(Actor);
		break;
	case EAIState::Combat:
		break;
	case EAIState::Frozen:
		break;
	case EAIState::OnFire:
		break;
	case EAIState::Immune:
		break;
	case EAIState::Dead:
		break;
	}
}
