// Written by jaegang lim


#include "Character/AI/AIController_Base.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Balian.h"
#include "Character/TCGASCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

const FName AAIController_Base::KEY_STATE(TEXT("State"));
const FName AAIController_Base::KEY_TARGET_ACTOR(TEXT("AttackTarget"));


AAIController_Base::AAIController_Base()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("Behavior Tree Component");
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

void AAIController_Base::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = CastChecked<ATCGASCharacter>(GetPawn());

	if (IsValid(BehaviorTree.Get()))
	{
		ATCGASCharacter* PlayerCharacter = CastChecked<ATCGASCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		RunBehaviorTree(BehaviorTree.Get());
		BehaviorTreeComponent->StartTree(*BehaviorTree.Get());
		BlackboardComponent->SetValueAsObject("SelfActor", OwnerPawn);
		BlackboardComponent->SetValueAsObject("AbilitySystem", OwnerPawn->GetAbilitySystemComponent());
		BlackboardComponent->SetValueAsObject(KEY_TARGET_ACTOR, PlayerCharacter);
		BlackboardComponent->SetValueAsObject("TargetAbilitySystem", PlayerCharacter->GetAbilitySystemComponent());
	}
}

void AAIController_Base::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(Blackboard.Get()) && IsValid(BehaviorTree.Get()))
	{
		Blackboard->InitializeBlackboard(*BehaviorTree.Get()->BlackboardAsset.Get());
	}
}

void AAIController_Base::OnUpdated(TArray<AActor*> const& UpdatedActors)
{
	FAIStimulus OutStimulus;
	for (AActor* Actor : UpdatedActors)
	{
		if (OwnerPawn->GetCharacterState() == ECharacterState::Combat || OwnerPawn->GetCharacterState() ==
			ECharacterState::Shocked)
		{
			return;;
		}

		if (CanSenseActor(Actor, EAISense::Sight, OutStimulus))
		{
			const ECharacterState OwnerState = OwnerPawn->GetCharacterState();
			// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black,
			//                                  FString::Printf(TEXT("새로운 시야 확인 Source : %s"), *GetPawn()->GetName()));
			if (Actor->IsA(ABalian::StaticClass()))
			{
				if (OwnerState == ECharacterState::Idle || OwnerState == ECharacterState::Patrolling || OwnerState ==
					ECharacterState::Investigating)
				{
					OwnerPawn->SetTargetingCharacter(Cast<ATCGASCharacter>(Actor));
					OwnerPawn->ChangeCharacterState(ECharacterState::Combat);
				}
				else
				{
					OwnerPawn->ChangeCharacterState(ECharacterState::Idle);
				}
			}
		}
		if (CanSenseActor(Actor, EAISense::Hearing, OutStimulus))
		{
			const ECharacterState OwnerState = OwnerPawn->GetCharacterState();
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow,
			                                 FString::Printf(TEXT("새로운 소리 확인 Source : %s"), *GetPawn()->GetName()));
			if (OwnerState != ECharacterState::Combat)
			{
				OwnerPawn->ChangeCharacterState(ECharacterState::Investigating);
				BlackboardComponent->SetValueAsVector("InvestigatingLocation", OutStimulus.StimulusLocation);
			}
		}
		if (CanSenseActor(Actor, EAISense::Damage, OutStimulus))
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
			                                 FString::Printf(TEXT("새로운 데미지 확인 Source : %s"), *GetPawn()->GetName()));
		}
	}
}

bool AAIController_Base::CanSenseActor(AActor* Actor, const EAISense SenseType, FAIStimulus& OutStimulus) const
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
