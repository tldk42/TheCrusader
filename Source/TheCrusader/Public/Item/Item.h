// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Item.generated.h"

class UDataTable;
class UItemBase;

UCLASS()
class THECRUSADER_API AItem : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AItem();

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
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }

protected:
	virtual void BeginPlay() override;

protected:
	void TakePickup(const ABalian* Taker);
	void UpdateInteractableData();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* OriginRoot;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Components")
	UStaticMeshComponent* Mesh;

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
