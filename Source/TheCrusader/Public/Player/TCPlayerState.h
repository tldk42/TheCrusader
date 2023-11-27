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

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StartInteractNPC(AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StopInteractNPC();
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

	virtual void BeginPlay() override;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);

public:
	FDelegateHandle HealthChangedDelegateHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = true))
	class UTCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuestSystem", meta = (AllowPrivateAccess = true))
	class UQuestManager* QuestManager;

	UPROPERTY()
	class UTCAttributeSet* AttributeSetBase;
};
