// Written by jaegang lim


#include "Character/Anim/NotifyState/WeaponAttack.h"

#include "AbilitySystemComponent.h"
#include "Character/TCGASCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void UWeaponAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	TArray<FHitResult> HitResults;

	FVector Start = MeshComp->GetSocketLocation(InSocket);
	FVector End = Start + (MeshComp->GetForwardVector() * Length);
	TArray<TEnumAsByte<EObjectTypeQuery>> Objects;
	Objects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));


	UKismetSystemLibrary::SphereTraceMultiForObjects(
		MeshComp,
		Start,
		End,
		Radius,
		Objects,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForDuration,
		HitResults,
		true);

	UE_LOG(LogTemp, Warning, TEXT("HIHT"));
	if (HitResults.Num() > 0)
	{
		for (auto& HitResult : HitResults)
		{
			ATCGASCharacter* Target = Cast<ATCGASCharacter>(HitResult.GetActor());
			
			// Target->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf()
		}
	}
}

void UWeaponAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// Super::NotifyEnd(MeshComp, Animation);
}
