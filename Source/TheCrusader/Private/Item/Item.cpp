// Written by jaegang lim


#include "Item/Item.h"

// template <typename ItemClass>
// AItem<ItemClass>::AItem()
// {
// 	PrimaryActorTick.bCanEverTick = false;
// 	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
// 	StaticMesh->SetSimulatePhysics(false);
// 	SetRootComponent(StaticMesh);
// }
//
// template <typename ItemClass>
// void AItem<ItemClass>::BeginFocus()
// {
// 	IInteractable::BeginFocus();
// }
//
// template <typename ItemClass>
// void AItem<ItemClass>::EndFocus()
// {
// 	IInteractable::EndFocus();
// }
//
// template <typename ItemClass>
// void AItem<ItemClass>::BeginInteract()
// {
// 	IInteractable::BeginInteract();
// }
//
// template <typename ItemClass>
// void AItem<ItemClass>::EndInteract()
// {
// 	IInteractable::EndInteract();
// }
//
// template <typename ItemClass>
// void AItem<ItemClass>::Interact(ABalian* PlayerCharacter)
// {
// 	IInteractable::Interact(PlayerCharacter);
// }
//
// template <typename ItemClass>
// void AItem<ItemClass>::BeginPlay()
// {
// 	AActor::BeginPlay();
// }
//
// template <typename ItemClass>
// void AItem<ItemClass>::UpdateInteractableData()
// {
// }
//
// template <typename ItemClass>
// void AItem<ItemClass>::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
// {
// 	Super::PostEditChangeProperty(PropertyChangedEvent);
//
// 	const FName ChangedPropertyName = PropertyChangedEvent.Property
// 										  ? PropertyChangedEvent.Property->GetFName()
// 										  : NAME_None;
//
// 	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(AItemPickup, DesiredItemID))
// 	{
// 		if (ItemDataTable)
// 		{
// 			const FString ContextString{DesiredItemID.ToString()};
//
// 			if (const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString()))
// 			{
// 				PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);
// 			}
// 		}
// 	}
// }
