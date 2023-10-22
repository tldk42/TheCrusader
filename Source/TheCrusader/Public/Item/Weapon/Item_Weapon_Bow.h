// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Item_Weapon.h"
#include "Item_Weapon_Bow.generated.h"

UCLASS()
class THECRUSADER_API AItem_Weapon_Bow : public AItem_Weapon
{
	GENERATED_BODY()

public:
	AItem_Weapon_Bow();

	FORCEINLINE USkeletalMeshComponent* GetBowComponent() const
	{
		return BowSkeletalMesh;
	};

protected:
	virtual void BeginPlay() override;

	virtual void InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity) override;
	virtual void InitializeDrop(UItemBase* ItemToDrop, int32 InQuantity) override;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Weapon | Components")
	USkeletalMeshComponent* BowSkeletalMesh;

public:
};
