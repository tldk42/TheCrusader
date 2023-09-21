// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemBase.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "ItemPickup.generated.h"

class UDataTable;

UCLASS(BlueprintType)
class THECRUSADER_API AItemPickup : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AItemPickup();

public:
#pragma region IInteractabale
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(ABalian* PlayerCharacter) override;
#pragma endregion IInteractabale

	virtual void InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity);
	virtual void InitializeDrop(UItemBase* ItemToDrop, int32 InQuantity);

	FORCEINLINE virtual UItemBase* GetItemData() const { return ItemRef; }

protected:
	virtual void BeginPlay() override;

protected:
	void TakePickup(const ABalian* Taker);
	void UpdateInteractableData();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Components")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	UDataTable* ItemDataTable;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	FName DesiredItemID;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	int32 ItemQuantity;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Interaction")
	FInteractableData InstanceInteractableData;
	
	UPROPERTY(VisibleAnywhere)
	UItemBase* ItemRef;
};
