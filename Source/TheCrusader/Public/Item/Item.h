// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Interfaces/SaveInterface.h"
#include "Item.generated.h"

class UCapsuleComponent;
class UDataTable;
class UItemBase;

UCLASS()
class THECRUSADER_API AItem : public AActor, public IInteractable, public ISaveInterface
{
	GENERATED_BODY()

public:
	AItem();

public:
#pragma region ISaveInterface

	virtual bool ShouldLoadTransform_Implementation() override;
	virtual void LoadActor_Implementation() override;

#pragma endregion ISaveInterface

#pragma region IInteractabale
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(ABalian* PlayerCharacter) override;
#pragma endregion IInteractabale

	virtual void InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity);
	virtual void InitializeDrop(UItemBase* ItemToDrop, int32 InQuantity);

	void DisableInteractionCollision() const;
	
	FORCEINLINE virtual UItemBase* GetItemData() const { return ItemRef; }
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return StaticMesh; }

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
	USceneComponent* SceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup | Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup | Components")
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup | Components")
	UCapsuleComponent* InteractionAreaCollision;

	UPROPERTY(EditAnywhere, Category = "Pickup | Item Initialization")
	UDataTable* ItemDataTable;

	UPROPERTY(EditAnywhere, Category = "Pickup | Item Initialization")
	FName DesiredItemID;

	UPROPERTY(EditAnywhere, Category = "Pickup | Item Initialization")
	int32 ItemQuantity;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Interaction")
	FInteractableData InstanceInteractableData;

	UPROPERTY(VisibleAnywhere)
	UItemBase* ItemRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
	bool bSpawn = true;
};
