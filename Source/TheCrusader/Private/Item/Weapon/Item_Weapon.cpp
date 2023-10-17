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
	CapsuleComponent->SetupAttachment(PickupMesh);

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

void AItem_Weapon::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
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
				ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
				// Enemy
				ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel3));
				// Player
				TArray<AActor*, FDefaultAllocator> IgnoreActors;
				IgnoreActors.Add(this);
				IgnoreActors.Add(OwnerPlayer);


				UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), PickupMesh->GetSocketLocation("start"),
				                                                  PickupMesh->GetSocketLocation("end"), 10.f,
				                                                  ObjectTypesArray,
				                                                  false, IgnoreActors,
				                                                  EDrawDebugTrace::ForDuration, HitResult, true);
				// UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), PickupMesh->GetSocketLocation("start"),
				//                                                   PickupMesh->GetSocketLocation("end"),
				//                                                   50.f, ObjectTypesArray, false, IgnoreActors,
				//                                                   EDrawDebugTrace::None, HitResult, true);
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
	// UKismetSystemLibrary::Delay(GetWorld(), .2f, FLatentActionInfo());
	bAttacked = false;
}

void AItem_Weapon::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemWeaponData* ItemData = ItemDataTable->FindRow<FItemWeaponData>(
			DesiredItemID, DesiredItemID.ToString());
		if (ItemData)
		{
			ItemRef = NewObject<UItemWeaponBase>(this, BaseClass);

			ItemRef->ID = ItemData->ItemID;
			ItemRef->ItemType = ItemData->ItemType;
			ItemRef->ItemQuality = ItemData->ItemQuality;
			ItemRef->ItemStatistics = ItemData->ItemStatistics;
			ItemRef->TextData = ItemData->TextData;
			ItemRef->NumericData = ItemData->NumericData;
			ItemRef->AssetData = ItemData->AssetData;
			Cast<UItemWeaponBase>(ItemRef)->WeaponData = ItemData->WeaponData;
			ItemRef->bIsPickup = true;

			InQuantity <= 0 ? ItemRef->SetQuantity(1) : ItemRef->SetQuantity(InQuantity);

			PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);

			UpdateInteractableData();
		}
	}
}
