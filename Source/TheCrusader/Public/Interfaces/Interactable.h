// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class ABalian;

UENUM(BlueprintType)
enum class EInteractableType : uint8
{
	Pickup,
	NonPlayerCharacter,
	Ride,
	Toggle,
	Container
};

USTRUCT()
struct FInteractableData
{
	GENERATED_BODY()

	FInteractableData()
		: InteractableType(EInteractableType::Pickup),
		  Quantity(0),
		  InteractionDuration(0)
	{
	}

	UPROPERTY(EditInstanceOnly)
	EInteractableType InteractableType;

	UPROPERTY(EditInstanceOnly)
	FText Name;

	UPROPERTY(EditInstanceOnly)
	FText Action;

	UPROPERTY(EditInstanceOnly)
	int8 Quantity;

	UPROPERTY(EditInstanceOnly)
	float InteractionDuration;
};

UINTERFACE()
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THECRUSADER_API IInteractable
{
	GENERATED_BODY()

public:
	virtual void BeginFocus();
	virtual void EndFocus();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(ABalian* PlayerCharacter);

	FInteractableData InteractableData;
};
