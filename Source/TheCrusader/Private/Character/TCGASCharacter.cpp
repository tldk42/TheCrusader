// Written by jaegang lim


#include "Character/TCGASCharacter.h"

#include <IVectorChangedEventArgs.h>
#include "MotionWarpingComponent.h"
#include "TheCrusader.h"
#include "Component/Physics/TCPhysicalAnimComp.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/TCAbilitySystemComponent.h"
#include "GAS/Ability/TCGameplayAbility.h"
#include "GAS/Attribute/TCAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ATCGASCharacter::ATCGASCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Front"));
	HitDirectionBackTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Back"));
	HitDirectionRightTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Right"));
	HitDirectionLeftTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Left"));
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

	PhysicalAnimComponent = CreateDefaultSubobject<UTCPhysicalAnimComp>("PhysicsAnimation");
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
	ForwardArrow = CreateDefaultSubobject<UArrowComponent>("ForwardArrow");
	BackArrow = CreateDefaultSubobject<UArrowComponent>("BackArrow");
	LeftArrow = CreateDefaultSubobject<UArrowComponent>("LeftArrow");
	RightArrow = CreateDefaultSubobject<UArrowComponent>("RightArrow");
	HandArrow = CreateDefaultSubobject<UStaticMeshComponent>("HandArrowMesh");
	HandArrow->SetVisibility(false);

	ForwardArrow->SetupAttachment(GetCapsuleComponent());
	BackArrow->SetupAttachment(GetCapsuleComponent());
	LeftArrow->SetupAttachment(GetCapsuleComponent());
	RightArrow->SetupAttachment(GetCapsuleComponent());
	HandArrow->SetupAttachment(GetMesh(), "arrow");

	UpdateMorphTargets();
	SetMasterPoseComponentForParts();

	PrimaryActorTick.bCanEverTick = false;
	bAlwaysRelevant = true;
}

#pragma region Ability System

UAbilitySystemComponent* ATCGASCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ATCGASCharacter::InitializeAttributes()
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."),
		       *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	if (const FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
		DefaultAttributes, 1, EffectContext); NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
			*NewHandle.Data.Get());
	}
}

void ATCGASCharacter::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->
		bStartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (const TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		if (FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
			GameplayEffect, 1, EffectContext); NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
				*NewHandle.Data.Get(), AbilitySystemComponent);
		}
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}

void ATCGASCharacter::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->
		bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UTCGameplayAbility>& StartupAbility : CharacterDefaultAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1,
			                     1, this));
	}

	for (const auto& MeleeAbility : MeleeAbilityMap)
	{
		FGameplayAbilitySpecHandle Spec = AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(
				MeleeAbility.Value, 1,
				1, this));
		MeleeAbilitySpec.Add(MeleeAbility.Key, Spec);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void ATCGASCharacter::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || !AbilitySystemComponent->
		bCharacterAbilitiesGiven)
	{
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterDefaultAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

#pragma endregion Ability System

#pragma region Getter

bool ATCGASCharacter::IsAlive() const
{
	return GetHealth() > 0.f;
}

float ATCGASCharacter::GetHealth() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float ATCGASCharacter::GetMaxHealth() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float ATCGASCharacter::GetStamina() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetStamina();
	}

	return 0.0f;
}

float ATCGASCharacter::GetMaxStamina() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetMaxStamina();
	}

	return 0.0f;
}

float ATCGASCharacter::GetPower() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetAttackPower();
	}

	return 0.0f;
}

float ATCGASCharacter::GetArmour() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetDefensePower();
	}

	return 0.0f;
}

AItem_Weapon_Bow* ATCGASCharacter::GetCurrentBow() const
{
	return CurrentBow;
}

UArrowComponent* ATCGASCharacter::GetWarpingInfo(const FVector HitLocation, bool& IsTooFar) const
{
	int8 Index = INT8_MAX;
	float ClosetDistance = 200.f;
	const float Forward = UKismetMathLibrary::Vector_Distance(HitLocation, ForwardArrow->GetComponentLocation());
	const float Back = UKismetMathLibrary::Vector_Distance(HitLocation, BackArrow->GetComponentLocation());
	const float Left = UKismetMathLibrary::Vector_Distance(HitLocation, LeftArrow->GetComponentLocation());
	const float Right = UKismetMathLibrary::Vector_Distance(HitLocation, RightArrow->GetComponentLocation());
	if (ClosetDistance > Forward)
	{
		ClosetDistance = Forward;
		Index = 0;
	}
	if (ClosetDistance > Back)
	{
		ClosetDistance = Back;
		Index = 1;
	}
	if (ClosetDistance > Left)
	{
		ClosetDistance = Left;
		Index = 2;
	}
	if (ClosetDistance > Right)
	{
		Index = 3;
	}

	if (ClosetDistance >= 200.f)
	{
		IsTooFar = true;
	}

	switch (Index)
	{
	case 0:
		return ForwardArrow;
	case 1:
		return BackArrow;
	case 2:
		return LeftArrow;
	case 3:
		return RightArrow;
	default:
		return ForwardArrow;
	}
}

ETCHitReactDirection ATCGASCharacter::GetHitReactDirection(const FVector& ImpactPoint) const
{
	const FVector& ActorLocation = GetActorLocation();

	const float DistanceToFrontBackPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorRightVector());
	const float DistanceToRightLeftPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorForwardVector());

	if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
	{
		if (DistanceToRightLeftPlane >= 0)
		{
			return ETCHitReactDirection::Front;
		}
		return ETCHitReactDirection::Back;
	}
	if (DistanceToFrontBackPlane >= 0)
	{
		return ETCHitReactDirection::Right;
	}
	return ETCHitReactDirection::Left;
}

#pragma endregion Getter

void ATCGASCharacter::PlayHitReact(const FGameplayTag HitDirection, AActor* DamageCauser)
{
	if (IsAlive())
	{
		if (HitDirection == HitDirectionLeftTag)
		{
			ShowHitReact.Broadcast(ETCHitReactDirection::Left);
		}
		else if (HitDirection == HitDirectionFrontTag)
		{
			ShowHitReact.Broadcast(ETCHitReactDirection::Front);
		}
		else if (HitDirection == HitDirectionRightTag)
		{
			ShowHitReact.Broadcast(ETCHitReactDirection::Right);
		}
		else if (HitDirection == HitDirectionBackTag)
		{
			ShowHitReact.Broadcast(ETCHitReactDirection::Back);
		}
	}
}

#pragma region Combat

void ATCGASCharacter::DoMeleeAttack()
{
	// 일시정지인지 다른 스킬을 사용하고있는지 살아있는지 확인해야함
	if (UGameplayStatics::IsGamePaused(GetWorld()))
		return;

	FGameplayTagContainer CheckThisTagContainer;
	CheckThisTagContainer.AddTag(FGameplayTag::RequestGameplayTag("Ability.Action.LightAttack"));

	TArray<UTCGameplayAbility*> GameplayAbilities;

	AbilitySystemComponent->GetActiveAbilitiesWithTags(CheckThisTagContainer, GameplayAbilities);

	// 이미 MeleeAttack을 진행중인가?
	if (GameplayAbilities.Num() > 0)
	{
		// 콤보를 업데이트한다.
		// JumpSectionForCombo();
		bComboEnabled = true;
	}
	else
	{
		const FName StartSections[2] = {"Combo1", "Combo1-2"};
		CurrentSectionName = StartSections[UKismetMathLibrary::RandomInteger(2)];

		if (const FMotionWarpingTarget* TargetData = MotionWarpingComponent->FindWarpTarget("Target"))
		{
			if (UKismetMathLibrary::Vector_Distance(TargetData->Location, GetActorLocation()) >= 280)
			{
				// const FName Sections[1] = {"LongD-1"};

				CurrentSectionName = TEXT("LongD-1");
			}
		}

		bComboEnabled = false;
		// 새로 몽타주를 재생한다.
		if (ActivateAbilitiesByWeaponType(CombatMode, true))
			if (ActivateAbilitiesByWeaponType(CombatMode, true))
			{
				if (CombatMode == EWeaponType::Idle)
				{
					// SetAnimLayer(EWeaponType::Boxer);
				}
			}
	}
}

void ATCGASCharacter::DoShoot()
{
	FGameplayTagContainer Container;
	Container.AddTag(FGameplayTag::RequestGameplayTag("Ability.Action.Shoot"));
	AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
}

void ATCGASCharacter::JumpSectionForCombo()
{
	// Check: 노티파이 구간안 그리고 콤보입력이 들어왔는지
	if (IsValid(JumpSectionNotify) && bEnableComboPeriod)
	{
		// 다음 섹션 후보 중에 랜덤으로 하나를 뽑는다. 
		const FName NextSectionName = JumpSectionNotify->JumpSections[FMath::RandRange(
			0, JumpSectionNotify->JumpSections.Num() - 1)];
		CurrentSectionName = NextSectionName;

		// 타겟과의 거리가 너무 멀다면 특정 공격 모션(멀리서부터 다가오는)으로 다음 섹션을 설정한다.
		if (const FMotionWarpingTarget* TargetData = MotionWarpingComponent->FindWarpTarget("Target"))
		{
			if (UKismetMathLibrary::Vector_Distance(TargetData->Location, GetActorLocation()) >= 280)
			{
				const FName Sections[2] = {"LongD-1", "LongD-2"};

				CurrentSectionName = Sections[UKismetMathLibrary::RandomInteger(2)];
			}
		}

		// Ability (공격을 시도한다.)
		ActivateAbilitiesByWeaponType(CombatMode, true);

		// Reset: 콤보 flag 초기화
		bEnableComboPeriod = false;
	}
}

bool ATCGASCharacter::ActivateAbilitiesByWeaponType(EWeaponType Mode, bool bAllowRemoteActivation)
{
	if (const FGameplayAbilitySpecHandle* FoundHandle = MeleeAbilitySpec.Find(Mode); FoundHandle &&
		AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbility(*FoundHandle, bAllowRemoteActivation);
	}
	return false;
}

#pragma endregion Combat

#pragma region Attribute Changes

void ATCGASCharacter::UpdateHealthBar() const
{
}

void ATCGASCharacter::Die()
{
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0.98f;
	GetCharacterMovement()->Velocity = FVector::Zero();

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagToRemove;
		EffectTagToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	if (TargetCharacter && TargetCharacter->GetTargetingCharacter() == this)
	{
		TargetCharacter->SetTargetingCharacter(nullptr);
	}

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		Destroy();
	}, 10.f, false);

	// if (DeadMontage)
	// {
	// 	PlayAnimMontage(DeadMontage);
	// 	// GetMesh()->SetSimulatePhysics(true);
	// 	PhysicalAnimComponent->TogglePhyxsAnimation();
	// }

	// FinishDying();
}

void ATCGASCharacter::FinishDying()
{
	Destroy();
}

void ATCGASCharacter::OnDamaged(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags,
                                ATCGASCharacter* InstigatorCharacter, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("DAMAGED"));
}

void ATCGASCharacter::OnHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	UpdateHealthBar();

	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		Die();
	}
}


void ATCGASCharacter::HandleDamage(const float DamageAmount, const FHitResult& HitInfo,
                                   const FGameplayTagContainer& DamageTags, ATCGASCharacter* InstigatorCharacter,
                                   AActor* DamageCauser)
{
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}

void ATCGASCharacter::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	OnHealthChanged(DeltaValue, EventTags);
}

#pragma endregion Attribute Changes
