// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCGASCharacter.h"
#include "EnemyBase.generated.h"

class UEnemyBar;
class UWidgetComponent;

UCLASS()
class THECRUSADER_API AEnemyBase : public ATCGASCharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

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

	UPROPERTY()
	UEnemyBar* InstancedWidget;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	UWidgetComponent* FloatingWidget;

private:
};
