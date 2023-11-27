// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Character/TCGASCharacter.h"
#include "Interfaces/Interactable.h"
#include "NPC_Base.generated.h"

UCLASS()
class THECRUSADER_API ANPC_Base : public ATCGASCharacter, public IInteractable
{
	GENERATED_BODY()

public:
	ANPC_Base(const FObjectInitializer& ObjectInitializer);

public:
#pragma region Interface Region

	/** IInteractable*/
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(ABalian* PlayerCharacter) override;

#pragma endregion Interface Region

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NPC Manager")
	class UNPCManager* NPCManager;

	UPROPERTY(EditInstanceOnly, Category = "Interact Data")
	FInteractableData InstancedInteractableData;
};
