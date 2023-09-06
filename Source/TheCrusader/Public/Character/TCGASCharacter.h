// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "TCCharacterBase.h"
#include "TCGASCharacter.generated.h"

UCLASS()
class THECRUSADER_API ATCGASCharacter : public ATCCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATCGASCharacter();

#pragma region GETTER

	UFUNCTION(BlueprintCallable, Category = "Crusader|BHCharacter")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|BHCharacter")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|BHCharacter")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|BHCharacter")
	float GetMaxStamina() const;

#pragma endregion GETTER
	
#pragma region IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
#pragma endregion IAbilitySystemInterface

	// 모든 어빌리티 제거 / 서버에서만 호출되고 서버에서 제거되면 클라이언트도 제거됨
	virtual void RemoveCharacterAbilities();

protected:
	
	virtual void AddCharacterAbilities();

	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

protected:
	UPROPERTY()
	class UTCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UTCAttributeSet* AttributeSetBase;

	// 죽으면 지워짐 / 리스폰 되면 다시 부여
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crusader|Abilities")
	TArray<TSubclassOf<class UTCGameplayAbility>> CharacterAbilities;

	// 스폰 / 리스폰될 때 부여되는 어트리뷰트
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crusader|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// 시작시에 나타나는 이펙트
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crusader|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;


	
};
