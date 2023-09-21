// Written by jaegang lim


#include "UI/Inventory/InventoryActionMenu.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/Balian.h"
#include "Component/Inventory/InventoryComponent.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Item/ItemPickup.h"
#include "Data/ItemDataStructs.h"
#include "Item/Data/ItemBase.h"
#include "Item/Weapon/MyItem.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Inventory/InventoryItemSlot.h"

void UInventoryActionMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UWidgetLayoutLibrary::SlotAsCanvasSlot(BoxActionMenu)->SetPosition(
		UWidgetLayoutLibrary::GetMousePositionOnViewport(this));

	BtnUse->OnClicked.AddDynamic(this, &ThisClass::Use);
	BtnDrop->OnClicked.AddDynamic(this, &ThisClass::Drop);
	BtnDropAll->OnClicked.AddDynamic(this, &ThisClass::DropAll);
}

void UInventoryActionMenu::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	RemoveFromParent();
}

void UInventoryActionMenu::SpawnItem(int32 Quantity) const
{
	UItemBase* Item = SourceSlot->ItemReference;
	if (Item)
	{
		if (Item->OwningInventory && Item->OwningInventory->FindMatchingItem(Item))
		{
			APawn* Pawn = GetOwningPlayerPawn();

			FHitResult HitResult;
			FVector Start =
				Pawn->GetActorLocation() +
				UKismetMathLibrary::RandomUnitVectorInConeInDegrees(
					Pawn->GetActorForwardVector(), 30.f) * 150.f;
			FVector End = Start - FVector(0, 0, 500.f);

			if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
			{
				const int32 RemovedQuantity = Item->OwningInventory->RemoveAmountOfItem(Item, Quantity);
				if (Item->ItemType == EItemType::Weapon)
				{
					AMyItem* WeaponItem = GetWorld()->SpawnActor<AMyItem>(
						AMyItem::StaticClass(), HitResult.Location, FRotator::ZeroRotator);
					WeaponItem->InitializeDrop(Item, RemovedQuantity);
				}
				else
				{
					AItemPickup* PickupItem = GetWorld()->SpawnActor<AItemPickup>(
						AItemPickup::StaticClass(), HitResult.Location, FRotator::ZeroRotator);
					PickupItem->InitializeDrop(Item, RemovedQuantity);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Can not remove item from inventory"));
		}
	}
}

void UInventoryActionMenu::Use()
{
	UItemBase* Item = SourceSlot->ItemReference;
	if (Item)
	{
		ABalian* Player = Cast<ABalian>(GetOwningPlayerPawn());
		if (Player)
		{
			Item->Use(Player);
		}

		Player->GetInventory()->RemoveAmountOfItem(Item, 1);
	}

	RemoveFromParent();
}

void UInventoryActionMenu::Drop()
{
	SpawnItem(1);
	RemoveFromParent();
}

void UInventoryActionMenu::DropAll()
{
	UItemBase* Item = SourceSlot->ItemReference;
	if (Item)
	{
		SpawnItem(Item->Quantity);
	}
	RemoveFromParent();
}
