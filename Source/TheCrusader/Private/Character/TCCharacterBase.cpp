#include "Character/TCCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Item/Data/ItemEquipmentBase.h"

ATCCharacterBase::ATCCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<USkeletalMeshComponent>("Body");
	Face = CreateDefaultSubobject<USkeletalMeshComponent>("Face");
	Beard = CreateDefaultSubobject<USkeletalMeshComponent>("Extra");
	Torso = CreateDefaultSubobject<USkeletalMeshComponent>("Torso");
	Arm = CreateDefaultSubobject<USkeletalMeshComponent>("Arm");
	Leg = CreateDefaultSubobject<USkeletalMeshComponent>("Leg");
	Feet = CreateDefaultSubobject<USkeletalMeshComponent>("Feet");
	Hair = CreateDefaultSubobject<USkeletalMeshComponent>("Hair");
	Hat = CreateDefaultSubobject<USkeletalMeshComponent>("Hat");

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>("Shield");
	SwordZip = CreateDefaultSubobject<UStaticMeshComponent>("SwordZip");
	LongswordZip = CreateDefaultSubobject<UStaticMeshComponent>("LongswordZip");
	Quiver = CreateDefaultSubobject<UStaticMeshComponent>("Quiver");

	Body->SetupAttachment(GetMesh());
	Face->SetupAttachment(GetMesh());
	Beard->SetupAttachment(GetMesh());
	Torso->SetupAttachment(GetMesh());
	Arm->SetupAttachment(GetMesh());
	Leg->SetupAttachment(GetMesh());
	Feet->SetupAttachment(GetMesh());
	Hair->SetupAttachment(GetMesh());
	Hat->SetupAttachment(GetMesh());

	ShieldMesh->SetupAttachment(GetMesh(), TEXT("shield_equip"));
	SwordZip->SetupAttachment(GetMesh(), TEXT("sword_equip"));
	LongswordZip->SetupAttachment(GetMesh(), TEXT("longsword_equip"));
	Quiver->SetupAttachment(GetMesh(), TEXT("QuiverSocket"));

	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordZip->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LongswordZip->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Quiver->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ShieldMesh->SetVisibility(false);
	SwordZip->SetVisibility(false);
	LongswordZip->SetVisibility(false);
	Quiver->SetVisibility(false);

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

void ATCCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ATCCharacterBase::SetMasterPoseComponentForParts() const
{
	Body->SetLeaderPoseComponent(GetMesh());
	Face->SetLeaderPoseComponent(GetMesh());
	Beard->SetLeaderPoseComponent(GetMesh());
	Torso->SetLeaderPoseComponent(GetMesh());
	Arm->SetLeaderPoseComponent(GetMesh());
	Leg->SetLeaderPoseComponent(GetMesh());
	Feet->SetLeaderPoseComponent(GetMesh());
	Hair->SetLeaderPoseComponent(GetMesh());
	Hat->SetLeaderPoseComponent(GetMesh());
}

float ATCCharacterBase::GetMorphTargetValue(const bool bCanMorph)
{
	return bCanMorph ? 1.f : 0.f;
}

void ATCCharacterBase::UpdateMorphTargets() const
{
	USkeletalMeshComponent* BodyParts[9];

	BodyParts[0] = Body;
	BodyParts[1] = Face;
	BodyParts[2] = Beard;
	BodyParts[3] = Torso;
	BodyParts[4] = Arm;
	BodyParts[5] = Leg;
	BodyParts[6] = Feet;
	BodyParts[7] = Hair;
	BodyParts[8] = Hat;

	for (USkeletalMeshComponent* Part : BodyParts)
	{
		if (Part)
		{
			if (UAnimInstance* AnimInstance = Part->GetAnimInstance())
			{
				AnimInstance->SetMorphTarget(TEXT("hat_equipped"), GetMorphTargetValue(bHatEquipped));
				AnimInstance->SetMorphTarget(TEXT("hat_equpped"), GetMorphTargetValue(bHatEquipped));
				AnimInstance->SetMorphTarget(TEXT("gloves_equipped"), GetMorphTargetValue(bGlovesEquipped));
			}
			else
			{
				Part->SetMorphTarget(TEXT("hat_equipped"), GetMorphTargetValue(bHatEquipped), true);
				Part->SetMorphTarget(TEXT("hat_equpped"), GetMorphTargetValue(bHatEquipped), true);
				Part->SetMorphTarget(TEXT("gloves_equipped"), GetMorphTargetValue(bGlovesEquipped), true);
			}
		}
	}
}

void ATCCharacterBase::AttachEquipment(const EEquipmentPart EquipmentPart, const UItemEquipmentBase* ItemToEquip)
{
	switch (EquipmentPart)
	{
	case EEquipmentPart::Head:
		bHatEquipped = true;
		Hat->SetSkeletalMesh(ItemToEquip->AssetData.AnimatedMesh);
		Hat->SetAnimInstanceClass(MorphAnimInstance);
		UpdateMorphTargets();
		break;
	case EEquipmentPart::Torso:
		Torso->SetSkeletalMesh(ItemToEquip->AssetData.AnimatedMesh);
		break;
	case EEquipmentPart::Hair:
		Hair->SetSkeletalMesh(ItemToEquip->AssetData.AnimatedMesh);
		Hair->SetAnimInstanceClass(MorphAnimInstance);
		break;
	case EEquipmentPart::Arm:
		Arm->SetSkeletalMesh(ItemToEquip->AssetData.AnimatedMesh);
		Arm->SetAnimInstanceClass(MorphAnimInstance);
		break;
	case EEquipmentPart::Legs:
		Leg->SetSkeletalMesh(ItemToEquip->AssetData.AnimatedMesh);
		break;
	case EEquipmentPart::Feet:
		Feet->SetSkeletalMesh(ItemToEquip->AssetData.AnimatedMesh);
		Feet->SetAnimInstanceClass(MorphAnimInstance);
		break;
	case EEquipmentPart::Shield:
		ShieldMesh->SetStaticMesh(ItemToEquip->AssetData.Mesh);
		ShieldMesh->SetVisibility(true);
		break;
	default: ;
	}

	// TODO: + 능력치 조정
}

void ATCCharacterBase::DetachEquipment(EEquipmentPart EquipmentPart)
{
	switch (EquipmentPart)
	{
	case EEquipmentPart::Head:
		break;
	case EEquipmentPart::Torso:
		break;
	case EEquipmentPart::Arm:
		break;
	case EEquipmentPart::Legs:
		break;
	case EEquipmentPart::Feet:
		break;
	case EEquipmentPart::Shield:
		ShieldMesh->SetVisibility(false);
		ShieldMesh->SetStaticMesh(nullptr);
		break;
	default: ;
	}
}
