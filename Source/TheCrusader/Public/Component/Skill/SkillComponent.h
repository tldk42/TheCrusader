// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SkillDataStruchts.h"
#include "SkillComponent.generated.h"


struct FPerksList;
class UDataTable;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THECRUSADER_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();

	UFUNCTION(BlueprintCallable)
	void SaveSkills();

	UFUNCTION(BlueprintCallable)
	bool LoadSkills();

	UFUNCTION(BlueprintCallable)
	void RestoreSavedSkills(const FDataTableRowHandle& Row);

	UFUNCTION(BlueprintNativeEvent)
	void LearnSkill(const FDataTableRowHandle& SkillRowHandle);

	UFUNCTION(BlueprintPure)
	static FSkill GetSkillInfo(const FDataTableRowHandle& SkillRowHandle);

#pragma region GETTER
	FORCEINLINE int GetAvailablePoint() const { return AvailablePoints; }
	FORCEINLINE TArray<FPerksList> GetPerksList() const { return PerksList; }
#pragma endregion GETTER

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyChanges(const FDataTableRowHandle& RowHandle);

private:
	void SetPerkToTrue(const FDataTableRowHandle& SkillRowHandle);

	void ControlPoints(const bool bPlus, const int Amount);

protected:
	UPROPERTY(EditDefaultsOnly)
	UDataTable* SkillDataTable;

	UPROPERTY(EditDefaultsOnly)
	TArray<FPerksList> PerksList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString SkillsSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int AvailablePoints;
};
