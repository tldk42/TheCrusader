// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillDataStruchts.generated.h"

class UTCGameplayAbility;

UENUM(BlueprintType)
enum class ESkillKeyType : uint8
{
	None,
	Active_1,
	Active_2,
	Active_3,
	Active_4,
	Active_5,
	Passive_1,
	Passive_2,
	Passive_3
};

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	Health,
	Stamina,
	Armor,
	HealthRegen,
	StaminaRegen,
	LightAttackPower,
	HardAttackPower,
	AbilitySkill,
	Magic,
	Other
};

UENUM(BlueprintType)
enum class ESkillValueType : uint8
{
	None,
	Plus,
	Percent,
	Infinite
};

USTRUCT(BlueprintType)
struct FPerksList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDataTableRowHandle RowHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bLearned;
};


USTRUCT(BlueprintType)
struct FSkill : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UTCGameplayAbility> AbilityClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESkillType SkillType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESkillValueType SkillValueType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* OriginIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* LearnedIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Value;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int RequiredPoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FDataTableRowHandle> RequiredSkills;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAllRequiredSkillsShouldBeLearned;
};

USTRUCT(BlueprintType)
struct FSavedSkill
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FPerksList> PerksLists;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int SkillPoints;
};

USTRUCT(BlueprintType)
struct FSavedSkillUIData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UTCGameplayAbility> AbilityClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAllocated;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* Icon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name;
};

USTRUCT(BlueprintType)
struct FSkillInformationWidgetBorder
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxY;
};
