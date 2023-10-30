// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemDataStructs.h"
#include "GameFramework/Character.h"
#include "TCCharacterBase.generated.h"


UCLASS(config=Game)
class ATCCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ATCCharacterBase(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable)
	virtual void AttachEquipment(const EEquipmentPart EquipmentPart, const UItemEquipmentBase* ItemToEquip);

	UFUNCTION(BlueprintCallable)
	virtual void DetachEquipment(EEquipmentPart EquipmentPart);

protected:
	virtual void BeginPlay() override;

	void SetMasterPoseComponentForParts() const;
	static float GetMorphTargetValue(bool bCanMorph);

	UFUNCTION(BlueprintCallable)
	void UpdateMorphTargets() const;

protected:
#pragma region Modular Parts

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Body;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Face;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Beard;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Torso;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Arm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Leg;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Feet;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Hair;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Hat;

#pragma endregion Modular Parts

#pragma region Equipment Accessory

	UPROPERTY(EditDefaultsOnly, Category = "Armour")
	UStaticMeshComponent* ShieldMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Armour")
	UStaticMeshComponent* LongswordZip;

	UPROPERTY(EditDefaultsOnly, Category = "Armour")
	UStaticMeshComponent* SwordZip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Armour", meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Quiver;
#pragma endregion Equipment Accessory


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> MorphAnimInstance;

	UPROPERTY(BlueprintReadWrite)
	bool bCrouching = false;

	bool bHatEquipped = false;

	bool bGlovesEquipped = false;
};
