// Written by jaegang lim


#include "Item/Item.h"

#include "Character/Balian.h"
#include "Component/Inventory/InventoryComponent.h"
#include "Components/CapsuleComponent.h"
#include "Game/TheCrusaderGameMode.h"
#include "Game/LoadScreenSaveGame.h"
#include "Item/Data/ItemBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Scene Component");
	SetRootComponent(SceneComponent);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	StaticMesh->SetupAttachment(SceneComponent);
	StaticMesh->SetSimulatePhysics(false);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Skeletal Mesh");
	SkeletalMeshComponent->SetupAttachment(SceneComponent);
	SkeletalMeshComponent->SetSimulatePhysics(false);

	InteractionAreaCollision = CreateDefaultSubobject<UCapsuleComponent>("Interaction Collision");
	InteractionAreaCollision->SetupAttachment(SceneComponent);
	InteractionAreaCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionAreaCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel2,
	                                                        ECR_Block);
}

bool AItem::ShouldLoadTransform_Implementation()
{
	return false;
}

void AItem::LoadActor_Implementation()
{
	if (!bSpawn)
	{
		Destroy();
	}
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
	if (StaticMesh)
	{
		StaticMesh->SetRenderCustomDepth(true);
	}
}

void AItem::EndInteract()
{
	if (StaticMesh)
	{
		StaticMesh->SetRenderCustomDepth(false);
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
		if (const FInventoryItem* ItemDTHandleRow = ItemDataTable->FindRow<FInventoryItem>(
			DesiredItemID, DesiredItemID.ToString()))
		{
			ItemRef = NewObject<UItemBase>(this, BaseClass);

			ItemRef->ItemData = *ItemDTHandleRow;
			ItemRef->bIsPickup = true;

			InQuantity <= 0 ? ItemRef->SetQuantity(1) : ItemRef->SetQuantity(InQuantity);

			// StaticMesh->SetStaticMesh(ItemDTHandleRow->AssetData.Mesh);

			UpdateInteractableData();
		}
	}
}

void AItem::InitializeDrop(UItemBase* ItemToDrop, int32 InQuantity)
{
	ItemRef = ItemToDrop;

	InQuantity <= 0 ? ItemRef->SetQuantity(1) : ItemRef->SetQuantity(InQuantity);

	// StaticMesh->SetStaticMesh(ItemToDrop->AssetData.Mesh);

	UpdateInteractableData();
}

void AItem::DisableInteractionCollision() const
{
	InteractionAreaCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

				if (const ATheCrusaderGameMode* TCGameMode = Cast<ATheCrusaderGameMode>(
					UGameplayStatics::GetGameMode(this)))
				{
					bSpawn = false;
					TCGameMode->SaveActor(this);
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
	InstanceInteractableData.Action = ItemRef->ItemData.TextData.InteractionText;
	InstanceInteractableData.Name = ItemRef->ItemData.TextData.Name;
	InstanceInteractableData.Quantity = ItemRef->Quantity;
	InteractableData = InstanceInteractableData;
}

void AItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (const FName ChangedPropertyName = PropertyChangedEvent.Property
		                                      ? PropertyChangedEvent.Property->GetFName()
		                                      : NAME_None; ChangedPropertyName == GET_MEMBER_NAME_CHECKED(
		AItem, DesiredItemID))
	{
		if (ItemDataTable)
		{
			const FString ContextString{DesiredItemID.ToString()};

			if (const FInventoryItem* ItemData = ItemDataTable->FindRow<FInventoryItem>(
				DesiredItemID, DesiredItemID.ToString()))
			{
				// StaticMesh->SetStaticMesh(ItemData->AssetData.Mesh);
			}
		}
	}
}
