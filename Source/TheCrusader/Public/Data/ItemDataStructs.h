// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TheCrusader.h"
#include "ItemDataStructs.generated.h"

class AItemPickup;

UENUM(BlueprintType)
enum class EItemQuality :uint8
{
	Shoddy,
	Common,
	Quality,
	Masterwork,
	Grandmaster
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Armor,
	Weapon,
	Shield,
	Spell,
	Consumable,
	Quest,
	Mundane
};

UENUM(BlueprintType)
enum class EEquipmentPart : uint8
{
	Head,
	Torso,
	Arm,
	Pants,
	Feet,
	Weapon,
	Shield,
	Bow
};

USTRUCT()
struct FItemStatistics
{
	GENERATED_BODY()

	FItemStatistics()
		: DamageValue(0), RestorationAmount(0), SellValue(0)
	{
	}

	UPROPERTY(EditAnywhere)
	float DamageValue;
	UPROPERTY(EditAnywhere)
	float RestorationAmount;
	UPROPERTY(EditAnywhere)
	float SellValue;
};

USTRUCT()
struct FEquipmentData
{
	GENERATED_BODY()

	FEquipmentData(): ArmorRatings(0)
	{
	}

	UPROPERTY(EditAnywhere)
	float ArmorRatings;

	UPROPERTY(EditAnywhere)
	FName AttachmentSocket;

	UPROPERTY(EditAnywhere)
	FName EquipmentSocket;
};

USTRUCT()
struct FItemTextData
{
	GENERATED_BODY()

	FItemTextData()
	{
	}

	UPROPERTY(EditAnywhere)
	FText Name;
	UPROPERTY(EditAnywhere)
	FText Description;
	UPROPERTY(EditAnywhere)
	FText InteractionText;
	UPROPERTY(EditAnywhere)
	FText UsageText;
};

USTRUCT()
struct FItemNumericData
{
	GENERATED_BODY()

	FItemNumericData() : Weight(0), MaxStackSize(0), bIsStackable(false)
	{
	}

	UPROPERTY(EditAnywhere)
	float Weight;
	UPROPERTY(EditAnywhere)
	int32 MaxStackSize;
	UPROPERTY(EditAnywhere)
	bool bIsStackable;
};

USTRUCT()
struct FItemAssetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AItemPickup> ItemClass;
};

USTRUCT()
struct FWeaponData
{
	GENERATED_BODY()

	FWeaponData(): Type(), AttachmentMontage(nullptr), EquipmentMontage(nullptr)
	{
	}

	UPROPERTY(EditAnywhere)
	EWeaponType Type;

	UPROPERTY(EditAnywhere)
	FName AttachmentSocket;

	UPROPERTY(EditAnywhere)
	FName EquipmentSocket;

	UPROPERTY(EditAnywhere)
	UAnimMontage* AttachmentMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* EquipmentMontage;
};

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ItemID;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemTextData TextData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemNumericData NumericData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemAssetData AssetData;
};

USTRUCT()
struct FItemEquipmentData : public FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Equipment Data")
	FEquipmentData EquipmentData;
	
	UPROPERTY(EditAnywhere, Category = "Equipment Data")
	EEquipmentPart EquipmentPart;
};

USTRUCT()
struct FItemWeaponData : public FItemEquipmentData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	FWeaponData WeaponData;
};
