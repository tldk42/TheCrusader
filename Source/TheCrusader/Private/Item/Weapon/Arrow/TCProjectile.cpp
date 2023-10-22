// Written by jaegang lim


#include "Item/Weapon/Arrow/TCProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"


ATCProjectile::ATCProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
}

void ATCProjectile::BeginPlay()
{
	Super::BeginPlay();
}
