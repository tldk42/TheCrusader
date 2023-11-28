// Written by jaegang lim


#include "Item/Weapon/Item_Weapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"


AItem_Weapon::AItem_Weapon()
	: bEquipped(false)
{
	PrimaryActorTick.bCanEverTick = false;
	ItemQuantity = 1;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	CapsuleComponent->SetupAttachment(SceneComponent);

	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

void AItem_Weapon::EventBeginWeaponAttack_Implementation(FGameplayTag EventTag)
{
	AttackEventTag = EventTag;
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AItem_Weapon::EventEndWeaponAttack_Implementation()
{
	CachedOverlappedPawns.Empty();
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItem_Weapon::BeginPlay()
{
	AActor::BeginPlay();
	if (!ItemRef)
	{
		InitializePickup(UItemWeaponBase::StaticClass(), 1);
	}
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItem_Weapon::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp,
                                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* OwnerPlayer = GetInstigator())
	{
		if (OtherActor->GetClass() != OwnerPlayer->GetClass())
		{
			if (!bAttacked && !CachedOverlappedPawns.Contains(OtherActor))
			{
				CachedOverlappedPawns.Add(OtherActor);

				FHitResult HitResult = SweepResult;

				TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
				ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
				// Enemy
				ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel3));
				// Player
				TArray<AActor*> IgnoreActors;
				IgnoreActors.Add(this);
				IgnoreActors.Add(OwnerPlayer);


				UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), StaticMesh->GetSocketLocation("start"),
				                                                  StaticMesh->GetSocketLocation("end"), 50.f,
				                                                  ObjectTypesArray,
				                                                  false, IgnoreActors,
				                                                  EDrawDebugTrace::ForOneFrame, HitResult, true);
				if (HitResult.bBlockingHit)
				{
					UE_LOG(LogTemp, Warning, TEXT("??"));
				}

				UAbilitySystemComponent* AbilitySystemComp =
					UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
						OwnerPlayer);
				FGameplayEffectContextHandle EffectContext = AbilitySystemComp->MakeEffectContext();
				FGameplayEventData Data;
				Data.Instigator = OwnerPlayer;
				Data.Target = OtherActor;
				Data.ContextHandle = EffectContext;
				Data.ContextHandle.AddHitResult(HitResult);
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerPlayer, AttackEventTag, Data);
				bAttacked = true;
			}
		}
	}
}

void AItem_Weapon::OnEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
                                class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bAttacked = false;
}
