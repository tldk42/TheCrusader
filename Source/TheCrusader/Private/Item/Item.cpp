// Written by jaegang lim


#include "Item/Item.h"

#include "Character/Balian.h"
#include "Component/Inventory/InventoryComponent.h"
#include "Item/Data/ItemBase.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	OriginRoot = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(OriginRoot);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	Mesh->SetupAttachment(OriginRoot);
	Mesh->SetSimulatePhysics(false);
}

void AItem::BeginFocus()
{
	IInteractable::BeginFocus();
}

void AItem::EndFocus()
{
	IInteractable::EndFocus();
}

void AItem::BeginInteract()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(true);
	}
}

void AItem::EndInteract()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(false);
	}
}

void AItem::Interact(ABalian* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		TakePickup(PlayerCharacter);
	}
}

void AItem::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString());

		if (ItemData)
		{
			ItemRef = NewObject<UItemBase>(this, BaseClass);

			ItemRef->ID = ItemData->ItemID;
			ItemRef->ItemType = ItemData->ItemType;
			ItemRef->ItemQuality = ItemData->ItemQuality;
			ItemRef->ItemStatistics = ItemData->ItemStatistics;
			ItemRef->TextData = ItemData->TextData;
			ItemRef->NumericData = ItemData->NumericData;
			ItemRef->AssetData = ItemData->AssetData;
			ItemRef->bIsPickup = true;

			InQuantity <= 0 ? ItemRef->SetQuantity(1) : ItemRef->SetQuantity(InQuantity);

			Mesh->SetStaticMesh(ItemData->AssetData.Mesh);

			UpdateInteractableData();
		}
	}
}

void AItem::InitializeDrop(UItemBase* ItemToDrop, int32 InQuantity)
{
	ItemRef = ItemToDrop;

	InQuantity <= 0 ? ItemRef->SetQuantity(1) : ItemRef->SetQuantity(InQuantity);

	Mesh->SetStaticMesh(ItemToDrop->AssetData.Mesh);

	UpdateInteractableData();
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (!ItemRef)
	{
		InitializePickup(UItemBase::StaticClass(), ItemQuantity);
	}
}

void AItem::TakePickup(const ABalian* Taker)
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

void AItem::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemRef->TextData.InteractionText;
	InstanceInteractableData.Name = ItemRef->TextData.Name;
	InstanceInteractableData.Quantity = ItemRef->Quantity;
	InteractableData = InstanceInteractableData;
}

void AItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property
		                                  ? PropertyChangedEvent.Property->GetFName()
		                                  : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(AItem, DesiredItemID))
	{
		if (ItemDataTable)
		{
			const FString ContextString{DesiredItemID.ToString()};

			if (const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString()))
			{
				Mesh->SetStaticMesh(ItemData->AssetData.Mesh);
			}
		}
	}
}
