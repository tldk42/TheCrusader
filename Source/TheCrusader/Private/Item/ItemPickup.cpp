// Written by jaegang lim


#include "Item/ItemPickup.h"

#include "Character/Balian.h"
#include "Component/Inventory/InventoryComponent.h"
#include "Item/ItemBase.h"


AItemPickup::AItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);
}

void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	InitializePickup(UItemBase::StaticClass(), ItemQuantity);
}

void AItemPickup::BeginFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(true);
	}
}

void AItemPickup::EndFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(false);
	}
}

void AItemPickup::BeginInteract()
{
}

void AItemPickup::EndInteract()
{
}

void AItemPickup::Interact(ABalian* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		TakePickup(PlayerCharacter);
	}
}

void AItemPickup::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString());

		ItemRef = NewObject<UItemBase>(this, BaseClass);

		ItemRef->ID = ItemData->ItemID;
		ItemRef->ItemType = ItemData->ItemType;
		ItemRef->ItemQuality = ItemData->ItemQuality;
		ItemRef->NumericData = ItemData->NumericData;
		ItemRef->TextData = ItemData->TextData;
		ItemRef->AssetData = ItemData->AssetData;

		InQuantity <= 0 ? ItemRef->SetQuantity(1) : ItemRef->SetQuantity(InQuantity);

		PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);

		UpdateInteractableData();
	}
}

void AItemPickup::InitializeDrop(UItemBase* ItemToDrop, int32 InQuantity)
{
	ItemRef = ItemToDrop;
	InQuantity <= 0 ? ItemRef->SetQuantity(1) : ItemRef->SetQuantity(InQuantity);
	ItemRef->NumericData.Weight = ItemToDrop->GetItemSingleWeight();
	PickupMesh->SetStaticMesh(ItemToDrop->AssetData.Mesh);
	UpdateInteractableData();
}

void AItemPickup::TakePickup(const ABalian* Taker)
{
	if (!IsPendingKillPending()) // ???
	{
		if (ItemRef)
		{
			if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
			{
				// 플레이어 인벤토리에 아이템 추가해야함
				const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemRef);

				switch (AddResult.OperationResult)
				{
				case EItemAddResult::NoItemAdded:
					break;
				case EItemAddResult::PartialAmountItemAdded:
					UpdateInteractableData();
					Taker->UpdateInteractionWidget();
					break;
				case EItemAddResult::AllTemAdded:
					Destroy();
					break;
				default: ;
				}

				UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Player inventory component is null"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup internal item reference was somehow null!"));
		}
	}
}

void AItemPickup::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemRef->TextData.InteractionText;
	InstanceInteractableData.Name = ItemRef->TextData.Name;
	InstanceInteractableData.Quantity = ItemRef->Quantity;
	InteractableData = InstanceInteractableData;
}

void AItemPickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property
		                                  ? PropertyChangedEvent.Property->GetFName()
		                                  : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(AItemPickup, DesiredItemID))
	{
		if (ItemDataTable)
		{
			const FString ContextString{DesiredItemID.ToString()};

			if (const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString()))
			{
				PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);
			}
		}
	}
}
