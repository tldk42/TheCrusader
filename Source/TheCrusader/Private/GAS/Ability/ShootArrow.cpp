// Written by jaegang lim


#include "GAS/Ability/ShootArrow.h"

#include "AbilitySystemComponent.h"
#include "Character/TCGASCharacter.h"
#include "GAS/TCAT_PlayMontageAndWaitForEvent.h"
#include "Item/Weapon/Item_Weapon_Bow.h"
#include "Item/Weapon/Arrow/TCProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UShootArrow::UShootArrow(): IdleShootMontage(nullptr), AimShootMontage(nullptr)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 어빌리티 태그 지정 (수정 할 수 있으므로 BP에서 진행)
	// 어빌리티 블록 태그 지정

	Range = 2000.0f;
	Damage = 120.0f;
}

void UShootArrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                  const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	UAnimMontage* MontageToPlay = IdleShootMontage;
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag(FName("State.Aiming"))))
	{
		MontageToPlay = AimShootMontage;
	}

	// Play fire montage and wait for event telling us to spawn the projectile
	UTCAT_PlayMontageAndWaitForEvent* Task = UTCAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
		this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &ThisClass::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &ThisClass::OnCancelled);
	Task->EventReceived.AddDynamic(this, &ThisClass::EventReceived);
	// ReadyForActivation() is how you activate the AbilityTask in C++. Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
}

void UShootArrow::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UShootArrow::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UShootArrow::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// Montage told us to end the ability before the montage finished playing.
	// Montage was set to continue playing animation even after ability ends so this is okay.
	// if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
	// {
	// 	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	// 	return;
	// }

	// Only spawn projectiles on the Server.
	// Predicting projectiles is an advanced topic not covered in this example.
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(
		FName("Event.Montage.SpawnProjectile")))
	{
		ATCGASCharacter* Hero = Cast<ATCGASCharacter>(GetAvatarActorFromActorInfo());
		if (!Hero)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		// FVector Start = Hero->GetCurrentBow()->GetBowComponent()->GetSocketLocation(FName("Muzzle"));
		// FVector End = Hero->GetCameraBoom()->GetComponentLocation() + Hero->GetFollowCamera()->GetForwardVector() *
		// 	Range;
		// FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

		const float MaxRange = FMath::RandRange(1.2f, 1.6f);
		float DamageMultiplier = FMath::Clamp(Hero->GetAimingDuration(), 1.f, MaxRange);

		FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(
			DamageGameplayEffect, GetAbilityLevel());

		// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
		DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(
			FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage * DamageMultiplier);

		Hero->InitAimingDuration();

		FVector MuzzleLocation = Hero->GetCurrentBow()->GetSkeletalMeshComp()->GetSocketLocation(FName("Muzzle"));
		// MuzzleTransform.SetRotation(MuzzleTransform.GetRotation().GetForwardVector().ToOrientationQuat());
		// MuzzleTransform.SetScale3D(FVector(1.0f));

		if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
		{
			USceneComponent* CameraRoot = CameraManager->GetRootComponent();


			FHitResult HitResult;
			GetWorld()->LineTraceSingleByChannel(
				HitResult,
				CameraRoot->GetComponentLocation(),
				CameraRoot->GetComponentLocation() + CameraRoot->GetForwardVector() * 20000.f,
				ECC_Visibility);

			FVector End = HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.TraceEnd;

			GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, End,
			                                     ECollisionChannel::ECC_GameTraceChannel1);

			FVector Temp = HitResult.bBlockingHit
				               ? HitResult.ImpactPoint
				               : HitResult.TraceEnd;

			FTransform ProjectileTransform(
				UKismetMathLibrary::MakeRotFromX(
					Temp - MuzzleLocation),
				MuzzleLocation,
				FVector::One());


			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ATCProjectile* Projectile = GetWorld()->SpawnActorDeferred<ATCProjectile>(
				ProjectileClass, ProjectileTransform, GetOwningActorFromActorInfo(),
				Hero, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
			Projectile->Range = Range;
			Projectile->FinishSpawning(ProjectileTransform);
		}
	}
}
