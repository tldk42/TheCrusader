// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Item_Pickup.generated.h"

UCLASS()
class THECRUSADER_API AItem_Pickup : public AItem
{
	GENERATED_BODY()

public:
	AItem_Pickup();

// public:
// 	virtual void InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity);
// 	virtual void InitializeDrop(UItemBase* ItemToDrop, int32 InQuantity);
//
// #pragma region IInteractabale
// 	virtual void Interact(ABalian* PlayerCharacter) override;
// #pragma endregion IInteractabale
//
// protected:
// 	virtual void BeginPlay() override;
//
// 	void TakePickup(const ABalian* Taker);
//
// #if WITH_EDITOR
// 	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
// #endif

	
public:
};
