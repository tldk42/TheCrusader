// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "TCCharacterBase.h"
#include "TheCrusader.h"
#include "Anim/NotifyState/JumpSection.h"
#include "TCGASCharacter.generated.h"

class AItem_Weapon_Bow;
class ADecalActor;
class UTCPhysicalAnimComp;
class AItem_Weapon;
struct FGameplayTag;
class USphereComponent;
enum class EWeaponType : uint8;

UENUM(BlueprintType)
enum class ETCHitReactDirection : uint8
{
	None,
	Left,
	Front,
	Right,
	Back,
	Up,
	Down
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitReactDelegate, ETCHitReactDirection, Direction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDiedDelegate, ATCGASCharacter*, Character);

UCLASS()
class THECRUSADER_API ATCGASCharacter : public ATCCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATCGASCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FHitReactDelegate ShowHitReact;

	UPROPERTY(BlueprintAssignable)
	FDiedDelegate OnCharacterDied;

	bool bComboEnabled;

#pragma region GETTER

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attribute")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attribute")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attribute")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attribute")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attribute")
	float GetPower() const;

	UFUNCTION(BlueprintCallable, Category = "Crusader|Attribute")
	float GetArmour() const;

	AItem_Weapon_Bow* GetCurrentBow() const;

	FORCEINLINE float GetAimingDuration() const
	{
		return AimingDuration;
	}

	UFUNCTION(BlueprintCallable)
	ETCHitReactDirection GetHitReactDirection(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	UArrowComponent* GetWarpingInfo(FVector HitLocation, bool& IsTooFar) const;

	UFUNCTION(BlueprintCallable)
	virtual void PlayHitReact(FGameplayTag HitDirection, AActor* DamageCauser);

	FORCEINLINE bool GetComboPeriod() const { return bEnableComboPeriod; }
	FORCEINLINE UJumpSection* GetJumpSectionNotify() const { return JumpSectionNotify; }

	FORCEINLINE ATCGASCharacter* GetTargetingCharacter() const { return TargetCharacter ? TargetCharacter : nullptr; }

	FORCEINLINE bool IsTargeting() const { return bIsTargeting; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

#pragma endregion GETTER

#pragma region SETTER

	FORCEINLINE void InitAimingDuration() { AimingDuration = 0.f; }
	FORCEINLINE void SetComboPeriod(const bool Value) { bEnableComboPeriod = Value; }
	FORCEINLINE void SetJumpSectionNotify(UJumpSection* JumpSection) { JumpSectionNotify = JumpSection; }
	FORCEINLINE void SetTargetingCharacter(ATCGASCharacter* Target) { TargetCharacter = Target; }

	UFUNCTION(BlueprintCallable, Category = "StateChanged")
	virtual void ChangeCharacterState(const ECharacterState NewState);

#pragma endregion SETTER

#pragma region IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
#pragma endregion IAbilitySystemInterface

	// 모든 어빌리티 제거 / 서버에서만 호출되고 서버에서 제거되면 클라이언트도 제거됨
	virtual void RemoveCharacterAbilities();

	void JumpSectionForCombo();

	UFUNCTION(BlueprintCallable)
	virtual void FinishDying();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayMantleMontage(UAnimMontage* MontageToPlay);

	UFUNCTION(BlueprintCallable)
	virtual void DoMeleeAttack();

	UFUNCTION(BlueprintCallable)
	virtual void DoShoot();

	virtual bool ActivateAbilitiesByWeaponType(EWeaponType Mode, bool bAllowRemoteActivation);

	virtual void AddCharacterAbilities();
	virtual void InitializeAttributes();
	virtual void AddStartupEffects();

	UFUNCTION(BlueprintCallable)
	virtual bool TryActivateAbilityByTag(const FGameplayTagContainer& TagContainer) const;

#pragma region Handle Damage & Health
	UFUNCTION(BlueprintCallable)
	virtual void OnDamaged(float DamageAmount, const FHitResult& HitInfo,
	                       const struct FGameplayTagContainer& DamageTags,
	                       ATCGASCharacter* InstigatorCharacter, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	virtual void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UFUNCTION(BlueprintCallable)
	virtual void OnStaminaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	virtual void UpdateHealthBar() const;
	virtual void UpdateStaminaBar() const;

	virtual void Die();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnDeath();

	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo,
	                          const struct FGameplayTagContainer& DamageTags, ATCGASCharacter* InstigatorCharacter,
	                          AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleStaminaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
#pragma endregion Handle Damage & Health

protected:
	UPROPERTY()
	class UTCAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	class UTCAttributeSet* AttributeSetBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UArrowComponent* ForwardArrow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UArrowComponent* BackArrow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UArrowComponent* LeftArrow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UArrowComponent* RightArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* HandArrow;

	UPROPERTY(BlueprintReadOnly)
	UTCPhysicalAnimComp* PhysicalAnimComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | MotionWarping")
	class UMotionWarpingComponent* MotionWarpingComponent;

	FGameplayTag HitDirectionFrontTag;
	FGameplayTag HitDirectionBackTag;
	FGameplayTag HitDirectionRightTag;
	FGameplayTag HitDirectionLeftTag;
	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	// 죽으면 지워짐 / 리스폰 되면 다시 부여
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crusader|Abilities")
	TArray<TSubclassOf<class UTCGameplayAbility>> CharacterDefaultAbilities;
	// 시작시에 나타나는 이펙트
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crusader|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Crusader|Abilities")
	TMap<EWeaponType, TSubclassOf<UTCGameplayAbility>> MeleeAbilityMap;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
	TMap<EWeaponType, FGameplayAbilitySpecHandle> MeleeAbilitySpec;

	// 스폰 / 리스폰될 때 부여되는 어트리뷰트
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crusader|Abilities")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Crusader|Abilities")
	TSubclassOf<UGameplayEffect> HUDMode;

	UPROPERTY(BlueprintReadWrite, Category = "Character | State")
	ECharacterState CharacterState = ECharacterState::Idle;

	UPROPERTY(BlueprintReadWrite, Category = "Character | State")
	EWeaponType CombatMode;

	UPROPERTY(BlueprintReadWrite, Category = "Character | State")
	bool bIsTargeting = false;

	UPROPERTY(BlueprintReadWrite, Category = "Character | State")
	float AimingDuration = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "Character | Target")
	ATCGASCharacter* TargetCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Character | Weapon")
	AItem_Weapon* CurrentWeapon;
	UPROPERTY(BlueprintReadOnly, Category = "Character | Weapon")
	AItem_Weapon_Bow* CurrentBow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character | Montage")
	FName CurrentSectionName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crusader | Montage")
	UJumpSection* JumpSectionNotify;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crusader | Montage")
	bool bEnableComboPeriod;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Montage | Asset")
	UAnimMontage* DeadMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Montage | HitReaction")
	ETCHitReactDirection HitReactDirection;

	friend UTCAttributeSet;
};
