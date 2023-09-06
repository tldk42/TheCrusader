// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCGASCharacter.h"
#include "GameFramework/Character.h"
#include "Balian.generated.h"

class UTCInputConfig;
struct FInputActionValue;

UCLASS()
class THECRUSADER_API ABalian : public ATCGASCharacter
{
	GENERATED_BODY()

public:
	ABalian();

	UFUNCTION(BlueprintPure)
	bool GetIsSprinting() const
	{
		return bIsSprinting;
	}

	/** Sets sprinting flag and calls OnIsSprintingChanged BIE if valid */
	UFUNCTION(BlueprintCallable)
	void SetIsSprinting(bool bNewIsSprinting);

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

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnIsSprintingChanged(bool bNewIsSprinting);

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void SpaceBarClick();
	void LMBClick();
	void RMBClick();
	void RMBCompleted();

private:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UTCInputConfig* InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* GASMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

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
