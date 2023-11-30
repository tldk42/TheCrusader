// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCGASCharacter.h"
#include "Interfaces/Interactable.h"
#include "TheCrusader.h"
#include "Interfaces/SavableCharacter.h"
#include "Balian.generated.h"

class USkillComponent;
class UTCMovementComponent;
class AInventoryPreview;
class UCameraComponent;
enum class EButtonType : uint8;
// Forward Declaration
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

UCLASS()
class THECRUSADER_API ABalian : public ATCGASCharacter, public ISavableCharacter
{
	GENERATED_BODY()

public:
	ABalian(const FObjectInitializer& ObjectInitializer);

	void LoadProgress();

	void UpdateInteractionWidget() const;
	bool UpdateStateByButton(EButtonType BtnType);

	void RemoveMappingContext() const;

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


#pragma endregion HandlingAttribute

	virtual void UpdateHealthBar() const override;
	virtual void UpdateStaminaBar() const override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnIsSprintingChanged(bool bNewIsSprinting);

	void FocusCameraToTarget();

	void EquipToHand(bool bMelee);
	void AttachToPelvis(bool bMelee);

	void SetVisibility_Accessory() const;

#pragma region Ability Function

	virtual bool ActivateAbilitiesByWeaponType(EWeaponType Mode, bool bAllowRemoteActivation) override;

	UFUNCTION(BlueprintCallable)
	void LockCamera();

	UFUNCTION(BlueprintCallable)
	void ReleaseCamera();

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
	void Ability1();
	void CrouchPressed();

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

private:
	void SpawnPreviewBalian();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess = true))
	UTCMovementComponent* TCMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Skill, meta = (AllowPrivateAccess = true))
	USkillComponent* SkillComponent;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	ATC_HUD* HUD;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	AHorse_Base* OwningHorse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<AInventoryPreview> PreviewCharacterClass;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	AInventoryPreview* PreviewCharacter;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DesiredSprintSpeed;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DesiredWalkSpeed;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DefaultWalkMinInputSize = 0.0f;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DefaultSprintMinInputSize = 0.5f;

	friend class AHorse_Base;
};
