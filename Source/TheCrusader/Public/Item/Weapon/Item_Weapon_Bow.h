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

	void PredictTarget();

	USkeletalMeshComponent* GetSkeletalMeshComp() const
	{
		return SkeletalMeshComponent;
	}

protected:
	virtual void BeginPlay() override;

private:
	FVector2D ScreenPos;
};
