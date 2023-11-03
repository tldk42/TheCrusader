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
	void HighlightBorder() const;

	UFUNCTION(BlueprintCallable)
	void UnHighlightBorder() const;

	UFUNCTION(BlueprintCallable)
	void ShowFloatingBar() const;
	UFUNCTION(BlueprintCallable)
	void HideFloatingBar() const;

protected:
	virtual void BeginPlay() override;

	virtual void Die() override;

	virtual void UpdateHealthBar() const override;

	virtual void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags,
	                       ATCGASCharacter* InstigatorCharacter, AActor* DamageCauser) override;

public:

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEnemyBar> WidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageText> DamageTextWidgetClass;

	UPROPERTY()
	UEnemyBar* InstancedWidget;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* FloatingWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* StealthLocation;

	UPROPERTY(EditInstanceOnly, Category = "Interact Data")
	FInteractableData InstancedInteractableData;

private:
};
