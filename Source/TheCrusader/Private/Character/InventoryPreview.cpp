// Written by jaegang lim


#include "Character/InventoryPreview.h"

#include "Character/Balian.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Item/Data/ItemEquipmentBase.h"
#include "Item/Data/ItemWeaponBase.h"


AInventoryPreview::AInventoryPreview(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");
	PreviewWeapon = CreateDefaultSubobject<UStaticMeshComponent>("PreviewWeapon");
	PreviewBow = CreateDefaultSubobject<USkeletalMeshComponent>("PreviewBow");

	SceneCaptureComponent2D->SetupAttachment(GetRootComponent());
	PreviewWeapon->SetupAttachment(GetMesh());
	PreviewBow->SetupAttachment(GetMesh(), "bow_equip");

	UpdateMorphTargets();
	SetMasterPoseComponentForParts();
	SetTickableWhenPaused(true);
	SceneCaptureComponent2D->SetTickableWhenPaused(true);
}

void AInventoryPreview::AttachEquipment(const EEquipmentPart EquipmentPart, const UItemEquipmentBase* ItemToEquip)
{
	Super::AttachEquipment(EquipmentPart, ItemToEquip);

	if (EquipmentPart == EEquipmentPart::Weapon)
	{
		if (const UItemWeaponBase* Weapon = Cast<UItemWeaponBase>(ItemToEquip))
		{
			PreviewWeapon->SetStaticMesh(ItemToEquip->AssetData.Mesh);
			switch (Weapon->WeaponData.Type)
			{
			case EWeaponType::TwoHandSword:
				PreviewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
				                                 "longsword_equip");
				break;
			case EWeaponType::OneHandSword:
				PreviewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
				                                 "sword_equip");
				break;
			case EWeaponType::Spear:
				PreviewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
				                                 "spear");
				break;
			case EWeaponType::Hammer:
				break;
			default: ;
			}
		}
	}
	else if (EquipmentPart == EEquipmentPart::Bow)
	{
		PreviewBow->SetSkeletalMesh(ItemToEquip->AssetData.AnimatedMesh);
	}
}

void AInventoryPreview::DetachEquipment(const EEquipmentPart EquipmentPart)
{
	Super::DetachEquipment(EquipmentPart);

	if (EquipmentPart == EEquipmentPart::Weapon)
	{
		PreviewWeapon->SetStaticMesh(nullptr);
	}
	else if (EquipmentPart == EEquipmentPart::Bow)
	{
		PreviewBow->SetSkeletalMesh(nullptr);
	}
}

void AInventoryPreview::BeginPlay()
{
	Super::BeginPlay();
}
