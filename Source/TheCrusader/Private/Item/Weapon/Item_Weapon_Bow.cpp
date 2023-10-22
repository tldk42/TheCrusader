// Written by jaegang lim


#include "Item/Weapon/Item_Weapon_Bow.h"
#include "Components/CapsuleComponent.h"


AItem_Weapon_Bow::AItem_Weapon_Bow()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BowSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("BowMesh");

	SetRootComponent(BowSkeletalMesh);
}

void AItem_Weapon_Bow::BeginPlay()
{
	Super::BeginPlay();
}

void AItem_Weapon_Bow::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	Super::InitializePickup(BaseClass, InQuantity);

	if (ItemRef && ItemRef->AssetData.AnimatedMesh)
	{
		BowSkeletalMesh->SetSkeletalMesh(ItemRef->AssetData.AnimatedMesh);
	}
}

void AItem_Weapon_Bow::InitializeDrop(UItemBase* ItemToDrop, int32 InQuantity)
{
	Super::InitializeDrop(ItemToDrop, InQuantity);

	if (ItemToDrop->AssetData.AnimatedMesh)
	{
		BowSkeletalMesh->SetSkeletalMesh(ItemToDrop->AssetData.AnimatedMesh);
		BowSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}
