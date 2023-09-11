// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

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

USTRUCT()
struct FItemStatistics
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float ArmorRatings;
	UPROPERTY(EditAnywhere)
	float DamageValue;
	UPROPERTY(EditAnywhere)
	float RestorationAmount;
	UPROPERTY(EditAnywhere)
	float SellValue;
};

USTRUCT()
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

USTRUCT()
struct FItemNumericData
{
	GENERATED_BODY()

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
