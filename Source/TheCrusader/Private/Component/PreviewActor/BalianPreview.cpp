// Written by jaegang lim


#include "Component/PreviewActor/BalianPreview.h"

#include "Components/SceneCaptureComponent2D.h"


ABalianPreview::ABalianPreview()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Root");

	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");
	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>("CharacterMesh");
	Face = CreateDefaultSubobject<USkeletalMeshComponent>("Face");
	Torso = CreateDefaultSubobject<USkeletalMeshComponent>("Torso");
	Arm = CreateDefaultSubobject<USkeletalMeshComponent>("Arm");
	Leg = CreateDefaultSubobject<USkeletalMeshComponent>("Leg");
	Feet = CreateDefaultSubobject<USkeletalMeshComponent>("Feet");

	SetRootComponent(SceneComponent);

	SceneCaptureComponent2D->SetupAttachment(GetRootComponent());
	CharacterMesh->SetupAttachment(GetRootComponent());
	Face->SetupAttachment(CharacterMesh);
	Torso->SetupAttachment(CharacterMesh);
	Arm->SetupAttachment(CharacterMesh);
	Leg->SetupAttachment(CharacterMesh);
	Feet->SetupAttachment(CharacterMesh);
}

void ABalianPreview::BeginPlay()
{
	Super::BeginPlay();
}
