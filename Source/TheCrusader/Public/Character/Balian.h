// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCGASCharacter.h"
#include "Data/SkillDataStruchts.h"
#include "Data/UIDataStructs.h"
#include "Interfaces/Interactable.h"
#include "Interfaces/SavableCharacter.h"
#include "Balian.generated.h"

#pragma region Forward Declaration

class UFootStepComponent;
class USkillComponent;
class UTCMovementComponent;
class AInventoryPreview;
class UCameraComponent;
enum class EButtonType : uint8;
class AHorse_Base;
class UItemEquipmentBase;
class UInputAction;
class UInputMappingContext;
class UTCPlayerCameraBehavior;
class UPlayerItemSlot;
class AItem_Weapon;
class UInventoryComponent;
class ATC_HUD;
class UTCInputConfig;
struct FInputActionValue;
enum class EEquipmentPart : uint8;

#pragma endregion Forward Declaration

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	Forward,
	Backward,
	Left,
	Right
};

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData()
		: CurrentInteractable(nullptr),
		  LastInteractionCheckTime(0.f)
	{
	}

	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillTriggered, const ESkillKeyType, const float)

UCLASS()
class THECRUSADER_API ABalian : public ATCGASCharacter, public ISavableCharacter
{
	GENERATED_BODY()

public:
	FOnSkillTriggered ActiveSkillTriggered;

public:
	ABalian(const FObjectInitializer& ObjectInitializer);

	void LoadProgress();

	void UpdateInteractionWidget() const;

	void RemoveMappingContext() const;

	UFUNCTION(BlueprintCallable)
	void LearnSkill(const FSkill& SkillInfo);

	UFUNCTION(BlueprintCallable)
	void AddSkill(const ESkillKeyType KeyType, const TSubclassOf<UTCGameplayAbility>& SKillAbility);

#pragma region AI Move TO
	UFUNCTION(BlueprintImplementableEvent)
	void MoveToLocation(FVector Location);
#pragma endregion AI Move TO

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StealthTakeDown();

#pragma region ISavableCharacter

	virtual int32 GetStatPoints_Implementation() const override;
	virtual int32 GetSkillPoints_Implementation() const override;
	virtual void LevelUp_Implementation() override;
	virtual void ShowSaveWidget_Implementation() const override;
	virtual void HideSaveWidget_Implementation() const override;
	virtual void SaveProgress_Implementation(const FName& CheckpointTag) override;

#pragma endregion ISavableCharacter

#pragma region GETTER

	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); }

	FORCEINLINE UInventoryComponent* GetInventory() const { return InventoryComponent; }

	FORCEINLINE AItem_Weapon* GetCurrentWeapon() const { return CurrentWeapon; }


	UFUNCTION(BlueprintPure)
	bool GetIsSprinting() const
	{
		return bIsSprinting;
	}

	UFUNCTION(BlueprintPure)
	float GetDesiredSprintSpeed() const { return DesiredSprintSpeed; }

	UFUNCTION(BlueprintPure)
	float GetDesiredWalkSpeed() const { return DesiredWalkSpeed; }

	UFUNCTION(BlueprintPure)
	float GetDesiredWalkMinInputSize() const
	{
		return DefaultWalkMinInputSize;
	}

	UFUNCTION(BlueprintPure)
	float GetDesiredSprintMinInputSize() const
	{
		return DefaultSprintMinInputSize;
	}

#pragma endregion GETTER

#pragma region SETTER

	/** Sets sprinting flag and calls OnIsSprintingChanged BIE if valid */
	UFUNCTION(BlueprintCallable)
	void SetIsSprinting(bool bNewIsSprinting);

	void SetAnimLayer();

	void SetCurrentWeapon(AItem_Weapon* Weapon);

	void SetCurrentBow(AItem_Weapon_Bow* Bow);

	virtual void AttachEquipment(const EEquipmentPart EquipmentPart, const UItemEquipmentBase* ItemToEquip) override;
	virtual void DetachEquipment(EEquipmentPart EquipmentPart) override;

	void SetOwningHorse(AHorse_Base* HorseToRide);
	FORCEINLINE void SetRiding(const bool bNewRiding)
	{
		bRiding = bNewRiding;
	}

#pragma endregion SETTER

protected:
#pragma region Character
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
#pragma endregion Character

#pragma region HandlingAttribute

	virtual void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags,
	                       ATCGASCharacter* InstigatorCharacter, AActor* DamageCauser) override;

	virtual void UpdateHealthBar() const override;
	virtual void UpdateStaminaBar() const override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnIsSprintingChanged(bool bNewIsSprinting);

#pragma endregion HandlingAttribute


#pragma region Ability Function

	virtual bool ActivateAbilitiesByWeaponType(EWeaponType Mode, bool bAllowRemoteActivation) override;

	UFUNCTION(BlueprintCallable)
	void LockCamera();

	UFUNCTION(BlueprintCallable)
	void ReleaseCamera();

	void FocusCameraToTarget();

#pragma endregion Ability Function

#pragma region InputBinding
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void SpaceBarClick();
	void LMBClick();
	void RMBClick();
	void RMBCompleted();
	void MMBClick();
	void Dodge();
	void Roll();
	void CrouchPressed();

	void ActivateSkill_1();
	void ActivateSkill_2();
	void ActivateSkill_3();
	void ActivateSkill_4();
	void ActivateSkill_5();
	void ActivatePassiveSkill_1();
	void ActivatePassiveSkill_2();
	void ActivatePassiveSkill_3();

#pragma endregion InputBinding

#pragma region Interact
	void PerformInteractionCheck();
	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void BeginInteract();
	void EndInteract();
	void Interact();
#pragma endregion Interact

#pragma region Inventory & Interaction Properties

	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction")
	TScriptInterface<IInteractable> TargetInteractable;

	UPROPERTY(VisibleAnywhere, Category = "Character | Inventory")
	UInventoryComponent* InventoryComponent;

	float InteractionCheckFrequency;

	float InteractionCheckDistance;

	FTimerHandle TimerHandle_Interaction;

	FInteractionData InteractionData;

#pragma endregion Inventory & Interaction Properties

	void ActivateSkillCoolDown(FGameplayAbilitySpecHandle SkillHandle) const;

	UFUNCTION()
	void RadialItemSelectedHandler(const FRadialMenuSendData& Data);

	UFUNCTION()
	void PassiveSkillLearnedHandler(const FRadialMenuItem& SkillButtonInfo);

private:
#pragma region Equip / Dettach / Preview
	void SpawnPreviewBalian();

	void SetVisibility_Accessory() const;

	void EquipToHand(bool bMelee);
	void AttachToPelvis(bool bMelee);
#pragma endregion Equip / Dettach / Preview

private:
#pragma region Animation (Montage, Layers)

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TMap<EWeaponType, TSubclassOf<UAnimInstance>> AnimLayers;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess))
	TMap<EWeaponType, UAnimMontage*> EquipMontages;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess))
	TMap<EWeaponType, UAnimMontage*> UnEquipMontages;

#pragma endregion Animation (Montage, Layers)

#pragma region Input & Action
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UTCInputConfig* InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* GASMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	ECardinalDirection InputDirection;

#pragma endregion Input & Action

#pragma region FLAGS

	bool bInventory = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	bool bRiding = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	bool bCanCounterAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	bool bCounterInputTriggered = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintSetter=SetIsSprinting, Meta=(AllowPrivateAccess="true"))
	bool bIsSprinting = false;

#pragma endregion FLAGS

#pragma region Input /  Speed
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DesiredSprintSpeed;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DesiredWalkSpeed;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DefaultWalkMinInputSize = 0.0f;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DefaultSprintMinInputSize = 0.5f;
#pragma endregion Input /  Speed

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DataTables", meta = (AllowPrivateAccess = true))
	UDataTable* DT_Menu_State;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DataTables", meta = (AllowPrivateAccess = true))
	UDataTable* DT_Menu_Skill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DataTables", meta = (AllowPrivateAccess = true))
	UDataTable* DT_Skill;

	UPROPERTY(BlueprintReadOnly, Category = "Crusader|Abilities|Skill", meta = (AllowPrivateAccess = true))
	TMap<FName, FGameplayAbilitySpecHandle> LearnedSkills;

	UPROPERTY(BlueprintReadOnly, Category = "Crusader|Abilities|Skill", meta = (AllowPrivateAccess = true))
	TMap<ESkillKeyType, FGameplayAbilitySpecHandle> SkillAbilityMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	UTCMovementComponent* TCMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = true))
	USkillComponent* SkillComponentS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FootStep", meta = (AllowPrivateAccess = true))
	UFootStepComponent* FootStepComponent;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	ATC_HUD* HUD;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AHorse_Base* OwningHorse;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AInventoryPreview* PreviewCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<AInventoryPreview> PreviewCharacterClass;

	friend class AHorse_Base;
};
