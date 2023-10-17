﻿// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCGASCharacter.h"
#include "Interfaces/Interactable.h"
#include "TheCrusader.h"
#include "Balian.generated.h"

class UItemEquipmentBase;
class UInputAction;
class UInputMappingContext;
class UTCPlayerCameraBehavior;
class UPlayerItemSlot;
class AItem_Weapon;
class AItemWeapon;
class UInventoryComponent;
class ATC_HUD;
class UTCInputConfig;
struct FInputActionValue;
enum class EEquipmentPart : uint8;

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
class THECRUSADER_API ABalian : public ATCGASCharacter
{
	GENERATED_BODY()

public:
	ABalian();

	void UpdateInteractionWidget() const;
	virtual void UpdateHealthBar() const override;

	void FocusCameraToTarget();
	
	void EquipToHand() const;
	void AttachToPelvis() const;

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

	bool SetAnimLayer(EWeaponType Mode);

	void SetCurrentWeapon(AItem_Weapon* Weapon);

	void AttachEquipment(EEquipmentPart EquipmentPart, UItemEquipmentBase* ItemToEquip);
	void DettachEquipment(EEquipmentPart EquipmentPart);

#pragma endregion SETTER

protected:
#pragma region Character
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
#pragma endregion Character

	UFUNCTION(BlueprintImplementableEvent)
	void OnIsSprintingChanged(bool bNewIsSprinting);

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
#pragma endregion InputBinding

#pragma region Interact
	void PerformInteractionCheck();
	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void BeginInteract();
	void EndInteract();
	void Interact();
#pragma endregion Interact

protected:
	UPROPERTY()
	ATC_HUD* HUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | MotionWarping")
	class UMotionWarpingComponent* MotionWarpingComponent;

#pragma region Inventory & Interaction Properties

	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction")
	TScriptInterface<IInteractable> TargetInteractable;

	UPROPERTY(VisibleAnywhere, Category = "Character | Inventory")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Character | Inventory", BlueprintReadOnly,
		meta = (AllowPrivateAccess = true))
	TMap<EEquipmentPart, UPlayerItemSlot*> ItemSlot;

	float InteractionCheckFrequency;

	float InteractionCheckDistance;

	FTimerHandle TimerHandle_Interaction;

	FInteractionData InteractionData;

#pragma endregion Inventory & Interaction Properties

private:

	UPROPERTY(EditDefaultsOnly, Category = "Armour")	
	UStaticMeshComponent* ShieldMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TMap<EWeaponType, TSubclassOf<UAnimInstance>> AnimLayers;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TMap<EWeaponType, UAnimMontage*> EquipMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TMap<EWeaponType, UAnimMontage*> AttachMontages;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	bool bIsTargeting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintSetter=SetIsSprinting, Meta=(AllowPrivateAccess="true"))
	bool bIsSprinting = false;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DesiredSprintSpeed;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DesiredWalkSpeed;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DefaultWalkMinInputSize = 0.0f;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
	float DefaultSprintMinInputSize = 0.5f;
};
