// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "TCPlayerState.generated.h"

struct FOnAttributeChangeData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGSOnGameplayAttributeValueChangedDelegate, FGameplayAttribute,
                                               Attribute, float, NewValue, float, OldValue);
/**
 * 
 */
UCLASS()
class THECRUSADER_API ATCPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATCPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UTCAttributeSet* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attributes")
	float GetHealthRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attributes")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attributes")
	float GetStaminaRegenRate() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = true))
	class UTCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UTCAttributeSet* AttributeSetBase;
	
	virtual void BeginPlay() override;

	// Attribute changed delegate handles
	FDelegateHandle HealthChangedDelegateHandle;
	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
};
