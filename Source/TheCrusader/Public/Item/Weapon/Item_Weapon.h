// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Item/ItemPickup.h"
#include "Item/Data/ItemWeaponBase.h"
#include "Item_Weapon.generated.h"

struct FGameplayTag;
class UCapsuleComponent;

UCLASS()
class THECRUSADER_API AItem_Weapon : public AItemPickup
{
	GENERATED_BODY()

public:
	AItem_Weapon();

	FORCEINLINE virtual UItemWeaponBase* GetItemData() const override
	{
		return Cast<UItemWeaponBase>(ItemRef);
	}

	virtual void InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EventBeginWeaponAttack(FGameplayTag EventTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EventEndWeaponAttack();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	bool bEquipped;
	bool bAttacked = false;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess =true))
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess =true))
	FGameplayTag AttackEventTag;

private:
	
	TArray<AActor*> CachedOverlappedPawns;
};
