// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Interactable.h"
#include "Horse_Base.generated.h"

class USphereComponent;
class ATCGASCharacter;

UCLASS()
class THECRUSADER_API AHorse_Base : public ACharacter, public IInteractable
{
	GENERATED_BODY()

	//----------------------------------------------
	// Functions
	// ---------------------------------------------
public:
	AHorse_Base();

	UFUNCTION(BlueprintCallable, Category = "Character|Horse")
	void TryMount(class ABalian* PlayerCharacter, UPrimitiveComponent* DirectionArrow);
	UFUNCTION(BlueprintCallable, Category = "Character|Horse")
	void TryUnMount();

#pragma region IInteractable
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(ABalian* PlayerCharacter) override;
#pragma endregion IInteractable

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
	                    class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepResult);

#pragma region Movement
	void Look(const FVector2D& Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);
#pragma endregion Movement

	void LookActionBinding(const struct FInputActionValue& ActionValue);
	void LookDeltaActionBinding(const struct FInputActionValue& ActionValue);
	void MoveActionBinding(const struct FInputActionValue& InputActionValue);
	void DismountActionBinding();

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//----------------------------------------------
	// Variables
	// ---------------------------------------------
protected:
	TObjectPtr<ABalian> Rider;

	UPROPERTY(EditInstanceOnly, Category = "Interact Data")
	FInteractableData InstancedInteractableData;

#pragma region Component
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UCapsuleComponent* InteractionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Reins;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Head;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Saddle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* FacialHair;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Pad;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UArrowComponent* LeftMountPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UArrowComponent* RightMountPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UCapsuleComponent* LeftArriveDetect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UCapsuleComponent* RightArriveDetect;

#pragma endregion Component


	// Mount : 0, 1 | Dismount : 2, 3 (left, right)
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	UAnimMontage* RiderMontages[4];

#pragma region InputMapping
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* LookControlsInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookDeltaAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* DismountAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 InputMappingPriority = 1;
#pragma endregion InputMapping

	float LastDeltaTime = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool bSprinting = false;

	bool bShouldStop;
	bool bShouldStopTurn;

	float BaseTurnRate = 30.f;
	float BaseLookUpRate = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = true))
	float InputForward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = true))
	float InputSide;
};
