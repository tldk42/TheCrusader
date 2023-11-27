// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCGASCharacter.h"
#include "Interfaces/Interactable.h"
#include "EnemyBase.generated.h"

class UDamageText;
class UEnemyBar;
class UWidgetComponent;

UCLASS()
class THECRUSADER_API AEnemyBase : public ATCGASCharacter, public IInteractable
{
	GENERATED_BODY()

public:
	AEnemyBase(const FObjectInitializer& ObjectInitializer);

#pragma region IInteractable
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(ABalian* PlayerCharacter) override;
#pragma endregion IInteractable

	UFUNCTION(BlueprintCallable)
	void SetupStartupWeapon(AItem_Weapon* Weapon);

	UFUNCTION(BlueprintCallable)
	void OnLocked() const;

	UFUNCTION(BlueprintCallable)
	void OnUnLocked() const;

	UFUNCTION(BlueprintCallable)
	void ShowFloatingBar() const;
	UFUNCTION(BlueprintCallable)
	void HideFloatingBar() const;

	virtual void ChangeCharacterState(const ECharacterState NewState) override;

protected:
	virtual void BeginPlay() override;

	virtual void Die() override;

	virtual void UpdateHealthBar() const override;
	virtual void UpdateStaminaBar() const override;

	virtual void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags,
	                       ATCGASCharacter* InstigatorCharacter, AActor* DamageCauser) override;

protected:
#pragma region Widget

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEnemyBar> FloatingWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LockWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageText> DamageTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UEnemyBar> StatusWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> LockWidget;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* FloatingWidgetComponent;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* LockWidgetComponent;

#pragma region Widget


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* StealthLocation;

	UPROPERTY(EditInstanceOnly, Category = "Interact Data")
	FInteractableData InstancedInteractableData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AItem_Weapon> WeaponClass;

private:
};
