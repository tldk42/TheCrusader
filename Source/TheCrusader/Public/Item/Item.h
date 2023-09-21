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
	// AItem();

// public:
// #pragma region IInteractabale
// 	virtual void BeginFocus() override;
// 	virtual void EndFocus() override;
// 	virtual void BeginInteract() override;
// 	virtual void EndInteract() override;
// 	virtual void Interact(ABalian* PlayerCharacter) override;
// #pragma endregion IInteractabale
//
// 	FORCEINLINE ItemClass* GetItemData() const { return ItemRef; }
//
// protected:
// 	virtual void BeginPlay() override;
//
// protected:
// 	void UpdateInteractableData();
//
// #if WITH_EDITOR
// 	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
// #endif
//
// protected:
// 	UPROPERTY(VisibleAnywhere, Category = "Item | Components")
// 	UStaticMeshComponent* StaticMesh;
//
// 	UPROPERTY(EditInstanceOnly, Category = "Item | Item Initialization")
// 	UDataTable* ItemDataTable;
//
// 	UPROPERTY(EditInstanceOnly, Category = "Item | Item Initialization")
// 	FName DesiredItemID;
//
// 	UPROPERTY(VisibleAnywhere, Category = "Item | Item Reference")
// 	ItemClass* ItemRef;
//
// 	UPROPERTY(EditInstanceOnly, Category = "Item | Item Initialization")
// 	int32 ItemQuantity;
//
// 	UPROPERTY(VisibleAnywhere, Category = "Item | Interaction")
// 	FInteractableData InstanceInteractableData;
};
