// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/ItemDataStructs.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

class UItemBase;
struct FInventoryItem;
class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT(BlueprintType)
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	UClass* Class;

	UPROPERTY(BlueprintReadWrite)
	bool bActor;

	UPROPERTY(BlueprintReadWrite)
	FName Name = FName();

	UPROPERTY(BlueprintReadWrite)
	FTransform Transform = FTransform();

	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> Data;
};

inline bool operator==(const FSavedActor& Left, const FSavedActor& Right)
{
	return Left.Name == Right.Name;
}

USTRUCT(BlueprintType)
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName = FString();

	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatus = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilitySlot = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel = 1;
};

inline bool operator==(const FSavedAbility& Left, const FSavedAbility* Right)
{
	return Left.AbilityTag.MatchesTagExact(Right->AbilityTag);
}

USTRUCT(BlueprintType)
struct FSavedInventory
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UItemBase> ItemDataClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FInventoryItem InventoryItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<uint8> Data;
};

/**
 * @ref https://docs.unrealengine.com/5.3/ko/saving-and-loading-your-game-in-unreal-engine/
 */
UCLASS()
class THECRUSADER_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** 슬롯 이름*/
	UPROPERTY()
	FString SlotName = FString();

	/** 저장된 세이브 슬롯의 번호*/
	UPROPERTY()
	int32 SlotIndex = 0;

	/** 플레이어 이름*/
	UPROPERTY()
	FString PlayerName;

	/** 로드될 맵 이름*/
	UPROPERTY()
	FString MapName;

	/** 로드될 맵의 어셋 이름*/
	UPROPERTY()
	FString MapAssetName;

	/** Start Tag*/
	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	FVector PlayerStartLocation;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = ESaveSlotStatus::Vacant;

	/** 첫 플레이 & 로드*/
	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	//========= Player =======================/
	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	float Health = 0;

	UPROPERTY()
	float Stamina = 0;

	UPROPERTY()
	int32 XP = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	float AttackPower = 0;

	UPROPERTY()
	float DefensePower = 0;

	UPROPERTY()
	float SkillPower = 0;

	//========= Abilities =======================/

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	UPROPERTY()
	TArray<FSavedInventory> SavedInventory;

	UPROPERTY()
	TArray<FSavedInventory> SavedEquipments;

	UFUNCTION(BlueprintCallable)
	void ActorArraySavor(UPARAM(ref) TArray<AActor*>& SaveActors);

	UFUNCTION(BlueprintCallable)
	void ActorSaver(AActor* SaveActor);

	UFUNCTION(BlueprintCallable)
	void UObjectSaver(UObject* SaveObject);

	UFUNCTION(BlueprintCallable)
	static void SaveData(UObject* Object, TArray<uint8>& Data);

	UFUNCTION(BlueprintCallable)
	static void LoadData(UObject* Object, UPARAM(ref) TArray<uint8>& Data);


	FSavedMap GetSavedMapWithMapName(const FString& InMapName, int& Index);
	bool HasMap(const FString& InMapName);
};
