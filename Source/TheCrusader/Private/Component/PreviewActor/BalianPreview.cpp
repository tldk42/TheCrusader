// Written by jaegang lim


#include "Component/PreviewActor/BalianPreview.h"

#include "Components/SceneCaptureComponent2D.h"


ABalianPreview::ABalianPreview()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Root");

	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");
	Body = CreateDefaultSubobject<USkeletalMeshComponent>("CharacterMesh");
	Face = CreateDefaultSubobject<USkeletalMeshComponent>("Face");
	Torso = CreateDefaultSubobject<USkeletalMeshComponent>("Torso");
	Arm = CreateDefaultSubobject<USkeletalMeshComponent>("Arm");
	Leg = CreateDefaultSubobject<USkeletalMeshComponent>("Leg");
	Feet = CreateDefaultSubobject<USkeletalMeshComponent>("Feet");
	Hair = CreateDefaultSubobject<USkeletalMeshComponent>("Hair");
	Hat = CreateDefaultSubobject<USkeletalMeshComponent>("Hat");

	SetRootComponent(SceneComponent);

	SceneCaptureComponent2D->SetupAttachment(GetRootComponent());
}

void ABalianPreview::BeginPlay()
{
	Super::BeginPlay();

	
}
