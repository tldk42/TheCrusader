#include "Character/TCCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

ATCCharacterBase::ATCCharacterBase()
{
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

	UpdateMorphTargets();
	SetMasterPoseComponentForParts();

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

void ATCCharacterBase::UpdateMorphTargets()
{
	USkeletalMeshComponent* BodyParts[8];

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
				AnimInstance->SetMorphTarget("hat_equipped", GetMorphTargetValue(bHatEquipped));
				AnimInstance->SetMorphTarget("gloves_equipped", GetMorphTargetValue(bGlovesEquipped));
			}
			else
			{
				Part->SetMorphTarget("hat_equipped", GetMorphTargetValue(bHatEquipped), true);
				Part->SetMorphTarget("gloves_equipped", GetMorphTargetValue(bGlovesEquipped), true);
			}
		}
	}
}
