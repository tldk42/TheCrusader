// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

class AItem_Interaction_Actor;
class AItem;

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
	Spell,
	Consumable,
	Quest,
	Mundane
};

UENUM(BlueprintType)
enum class EEquipmentPart : uint8
{
	None,
	Weapon,
	Bow,
	Hair,
	Head,
	Torso,
	Arm,
	Legs,
	Feet,
	Shield,
};

UENUM(BlueprintType, Blueprintable)
enum class EWeaponType : uint8
{
	None,
	Boxer,
	TwoHandSword,
	OneHandSword,
	SwordWithShield,
	Spear,
	Hammer,
	Bow
};

/** Item Statistics
 * Damaged value
 * Restore Value
 * Sell Value
 */
USTRUCT(BlueprintType)
struct FItemStatistics
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float DamageValue = 0;
	UPROPERTY(EditAnywhere)
	float RestorationAmount = 0;
	UPROPERTY(EditAnywhere)
	float SellValue = 0;
};

/** Attachment Data
 * Equip Socket Name
 * UnEquip Socket Name
 */
USTRUCT(BlueprintType)
struct FAttachmentData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int8 RequiredLevel = 0.f;

	UPROPERTY(EditAnywhere)
	EEquipmentPart EquipmentPart;

	UPROPERTY(EditAnywhere)
	FName AttachmentSocket;

	UPROPERTY(EditAnywhere)
	FName DetachmentSocket;
};

/** Numeric Data
 * Weight
 * StackSize
 * Can Stack
 */
USTRUCT(BlueprintType)
struct FItemNumericData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Weight = 0;
	UPROPERTY(EditAnywhere)
	int32 MaxStackSize = 1;
	UPROPERTY(EditAnywhere)
	bool bIsStackable = false;
};

/** Item Text Data
 * Name Text,
 * Description Text,
 * Interaction Text,
 * Usage Text
 */
USTRUCT(BlueprintType)
struct FItemTextData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText Name;
	UPROPERTY(EditAnywhere)
	FText Description;
	UPROPERTY(EditAnywhere)
	FText InteractionText;
	UPROPERTY(EditAnywhere)
	FText UsageText;
};

/** Item Asset Data
 * Icon Texture
 * Actual Class
 */
USTRUCT(BlueprintType)
struct FItemAssetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AItem_Interaction_Actor> ItemToInteract;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AItem> ItemToUse;

	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	UStaticMesh* InteractionMesh;
};

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EWeaponType Type;

	UPROPERTY(EditAnywhere)
	float BaseDamage = 1.f;

	UPROPERTY(EditAnywhere)
	float Strength = 1.f;
};

USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FDataTableRowHandle TableRow;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ItemID;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemAssetData AssetData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemNumericData NumericData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemTextData TextData;
	
	UPROPERTY(EditAnywhere, Category = "Equipment Data")
	FAttachmentData EquipmentData;

	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	FWeaponData WeaponData;
};

USTRUCT(BlueprintType)
struct FInventory_Equipment : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Equipment Data")
	FAttachmentData EquipmentData;

	UPROPERTY(EditAnywhere, Category = "Equipment Data")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, Category = "Equipment Data")
	UStaticMesh* InteractionMesh;

	UPROPERTY(EditAnywhere, Category = "Equipment Data")
	float DefenseRate = 0;
};

USTRUCT(BlueprintType)
struct FMyStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	FWeaponData WeaponData;

	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	FAttachmentData EquipmentData;

	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	UStaticMesh* InteractionMesh;
};
